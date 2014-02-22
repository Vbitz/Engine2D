#include "Events.hpp"

#include <vector>
#include <unordered_map>

#include "Logger.hpp"

namespace Engine {
    namespace Events {
        
        // It's just the v8 code fitted closer to the engine's coding style
        static void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch) {
            std::stringstream ss;
            ss << "Error In C++ to JavaScript Call: ";
            v8::HandleScope handle_scope(isolate);
            v8::String::Utf8Value exception(try_catch->Exception());
            const char* exception_string = *exception;
            v8::Handle<v8::Message> message = try_catch->Message();
            if (message.IsEmpty()) {
                // V8 didn't provide any extra information about this error; just
                // print the exception.
                ss << exception_string << std::endl;
            } else {
                // Print (filename):(line number): (message).
                v8::String::Utf8Value filename(message->GetScriptResourceName());
                const char* filename_string = *filename;
                int linenum = message->GetLineNumber();
                ss << filename_string << ":" << linenum << ": " << exception_string << std::endl;
                // Print line of source code.
                v8::String::Utf8Value sourceline(message->GetSourceLine());
                const char* sourceline_string = *sourceline;
                ss << sourceline_string << std::endl;
                // Print wavy underline (GetUnderline is deprecated).
                int start = message->GetStartColumn();
                for (int i = 0; i < start; i++) {
                    ss << " ";
                }
                int end = message->GetEndColumn();
                for (int i = start; i < end; i++) {
                    ss << "^";
                }
                v8::String::Utf8Value stack_trace(try_catch->StackTrace());
                if (stack_trace.length() > 0) {
                    const char* stack_trace_string = *stack_trace;
                    ss << std::endl << stack_trace_string;
                }
            }
            Logger::LogText("Scripting", Logger::LogLevel_Error, ss.str());
        }
        
        class EventTarget {
        public:
            void Run(std::function<bool(Json::Value)> filter, Json::Value e) {
                if (filter(_filter)) {
                    _run(e);
                }
            }
            
            virtual bool IsScript() { return true; }
            
        protected:
            Json::Value _filter = Json::nullValue;
            
            void setFilter(Json::Value filter) {
                _filter = filter;
            }
            
            virtual bool _run(Json::Value e) { return false; }
        };
        
        class CPPEventTarget : public EventTarget {
        public:
            CPPEventTarget(EventTargetFunc func, Json::Value* filter)
                : _func(func) {
                    setFilter(filter);
                }
            
        protected:
            bool _run(Json::Value e) {
                this->_func(e);
                return true;
            }
            
        private:
            EventTargetFunc _func;
        };
        
        class JSEventTarget : public EventTarget {
        public:
            JSEventTarget(v8::Persistent<v8::Function>* func, Json::Value* filter)
                : _func(func) {
                    setFilter(filter);
                }
            
        protected:
            bool _run(Json::Value e) {
                v8::HandleScope scp(v8::Isolate::GetCurrent());
                v8::Local<v8::Context> ctx = v8::Isolate::GetCurrent()->GetCurrentContext();
                if (ctx.IsEmpty() || ctx->Global().IsEmpty()) return;
                v8::Context::Scope ctx_scope(ctx);
                
                v8::TryCatch tryCatch;
                
                v8::Handle<v8::Value> args[1];
                
                v8::Handle<v8::Object> obj = ScriptingManager::GetObjectFromJson(e);
                
                args[0] = obj;
                
                v8::Handle<v8::Function> func = v8::Handle<v8::Function>::New(v8::Isolate::GetCurrent(), *_func);
                
                func->Call(ctx->Global(), 1, args);
            
                if (!tryCatch.StackTrace().IsEmpty()) {
                    ReportException(v8::Isolate::GetCurrent(), &tryCatch);
                    return false;
                } else {
                    return true;
                }
            }
            
        private:
            v8::Persistent<v8::Function>* _func;
        };
        
        struct Event {
            std::string Label;
            EventTarget* Target = NULL;
            bool Active = false;
            
            Event() : Label(""), Active(false) {}
            Event(std::string Label, EventTarget* target) : Label(Label), Target(target), Active(true) { }
        };
        
        struct EventClassSecurity {
            bool NoScript = false;
        };
        
        struct EventClass {
            bool Valid = false;
            std::string TargetName;
            EventClassSecurity Security;
            std::unordered_map<std::string, Event> Events;
        };
        
        std::unordered_map<std::string, EventClass> _events;
        
        int lastEventID = 0;
        
        void Emit(std::string evnt, std::function<bool(Json::Value)> filter, Json::Value args) {
            std::vector<Event> targets;
            EventClass& cls = _events[evnt];
            if (!cls.Valid) return;
            for (auto iter = cls.Events.begin(); iter != cls.Events.end(); iter++) {
                targets.push_back(iter->second);
            }
            for (auto iter = targets.begin(); iter != targets.end(); iter++) {
                if (iter->Target == NULL) { throw "Invalid Target"; }
                if (iter->Active) {
                    iter->Target->Run(filter, args);
                } else {
                    std::cout << "Deleting: " << iter->Label << std::endl;
                    delete cls.Events[iter->Label].Target;
                    cls.Events.erase(iter->Label);
                }
            }
        }
        
        void Emit(std::string evnt, Json::Value args) {
            Emit(evnt, [](Json::Value e) { return true; }, args);
        }
        
        void Emit(std::string evnt) {
            Emit(evnt, [](Json::Value e) { return true; }, Json::nullValue);
        }
        
        void On(std::string evnt, std::string name, Json::Value e, EventTargetFunc target) {
            std::string evnt_copy = std::string(evnt.c_str());
            std::string name_copy = std::string(name.c_str());
            Event newEvent = Event(name_copy, new CPPEventTarget(target, new Json::Value(e)));
            EventClass& cls = _events[evnt_copy];
            if (!cls.Valid) {
                cls.Valid = true;
                cls.TargetName = evnt_copy;
            }
            cls.Events[name_copy] = newEvent;
        }
        
        void On(std::string evnt, std::string name, Json::Value e, v8::Persistent<v8::
            Function>* target) {
            std::string evnt_copy = std::string(evnt.c_str());
            std::string name_copy = std::string(name.c_str());
            Event newEvent = Event(name_copy, new JSEventTarget(target, new Json::Value(e)));
            EventClass& cls = _events[evnt_copy];
            if (!cls.Valid) {
                cls.Valid = true;
                cls.TargetName = evnt_copy;
            }
            cls.Events[name_copy] = newEvent;
        }
        
        void On(std::string evnt, std::string name, EventTargetFunc target) {
            On(evnt, name, Json::nullValue, target);
        }
        
        void On(std::string evnt, std::string name, v8::Persistent<v8::Function>* target) {
            On(evnt, name, Json::nullValue, target);
        }
        
        void Clear(std::string eventID) {
            for (auto iter = _events.begin(); iter != _events.end(); iter++) {
                for (auto iter2 = iter->second.Events.begin();
                     iter2 != iter->second.Events.end(); iter2++) {
                    if (iter2->first == eventID) {
                        iter2->second.Active = false;
                    }
                }
            }
        }
    }
}