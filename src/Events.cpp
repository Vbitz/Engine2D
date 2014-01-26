#include "Events.hpp"

#include <vector>

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
            void Run(std::function<bool(EventArgs)> filter, EventArgs e) {
                if (filter(_filter)) {
                    _run(e);
                }
            }
            
        protected:
            EventArgs _filter;
            
            void setFilter(EventArgs filter) {
                _filter = filter;
            }
            
            virtual bool _run(EventArgs e) { return false; }
        };
        
        class CPPEventTarget : public EventTarget {
        public:
            CPPEventTarget(EventTargetFunc func, EventArgs filter)
                : _func(func) {
                    setFilter(filter);
                }
            
        protected:
            bool _run(EventArgs e) {
                this->_func(e);
                return true;
            }
            
        private:
            EventTargetFunc _func;
        };
        
        class JSEventTarget : public EventTarget {
        public:
            JSEventTarget(v8::Persistent<v8::Function>* func, EventArgs filter)
                : _func(func) {
                    setFilter(filter);
                }
            
        protected:
            bool _run(EventArgs e) {
                v8::HandleScope scp(v8::Isolate::GetCurrent());
                v8::Local<v8::Context> ctx = v8::Isolate::GetCurrent()->GetCurrentContext();
                v8::Context::Scope ctx_scope(ctx);
                
                v8::TryCatch tryCatch;
                
                v8::Handle<v8::Value> args[1];
                
                v8::Handle<v8::Object> obj = e.getObject();
                
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
        
        EventArgs::EventArgs(std::map<std::string, std::string> map)  : _readOnly(true) {
            for (auto iter = map.begin(); iter != map.end(); iter++) {
                this->_map[iter->first] = iter->second;
            }
        }
        
        EventArgs::EventArgs(v8::Handle<v8::Object> obj) : _readOnly(true) {
            v8::Local<v8::Array> objNames = obj->GetPropertyNames();
            
            for (int i = 0; i < objNames->Length(); i++) {
                v8::Local<v8::String> objKey = objNames->Get(i)->ToString();
                v8::Local<v8::Value> objItem = obj->Get(objKey);
                std::string objKeyValue = std::string(*v8::String::Utf8Value(objKey));
                this->_map[objKeyValue] = std::string(*v8::String::Utf8Value(objItem->ToString()));
            }
        }
        
        bool EventArgs::equals(const Engine::Events::EventArgs &rhs) {
            for (auto iter = this->_map.begin(); iter != this->_map.end(); iter++) {
                if (rhs._map.count(iter->first) == 0){
                    return false;
                } else {
                    if (iter->second != rhs.get(iter->first)) {
                        return false;
                    }
                }
            }
            return true;
        }
        
        bool EventArgs::equalsValue(std::string key, std::string value, bool required) {
            if (this->_map.count(key) == 0 && !required) {
                return true;
            } else if (this->_map.count(key) > 0 && this->get(key) == value) {
                return true;
            } else {
                return false;
            }
        }
        
        v8::Handle<v8::Object> EventArgs::getObject() {
            v8::HandleScope scp(v8::Isolate::GetCurrent());
                
            v8::Handle<v8::Object> ret = v8::Object::New();
            
            for (auto iter = this->_map.begin(); iter != this->_map.end(); iter++) {
                ret->Set(v8::String::New(iter->first.c_str()),
                        v8::String::New(iter->second.c_str()));
            }
            
            return scp.Close(ret);
        }
        
        std::string EventArgs::get(std::string key) const {
            if (this->_map.count(key) == 0) {
                return "";
            } else {
                return this->_map.at(key);
            }
        }
        
        void EventArgs::push(std::string key, std::string value) {
            if (!this->_readOnly) {
                this->_map[key] = value;
            }
        }
        
        struct Event {
            std::string TargetName, Label;
            EventTarget* Target;
            bool Active;
            
            Event(std::string targetName, std::string label, EventTarget* target) {
                this->TargetName = targetName;
                this->Label = label;
                this->Target = target;
                this->Active = true;
            }
        };
        
        std::vector<Event> _events;
        
        int lastEventID = 0;
        
        void Emit(std::string evnt, std::function<bool(EventArgs)> filter, EventArgs args) {
            std::vector<Event> targets;
            for (auto iter = _events.begin(); iter != _events.end(); iter++) {
                if (iter->TargetName == evnt && iter->Active) {
                    targets.push_back(*iter);
                }
            }
            for (auto iter = targets.begin(); iter != targets.end(); iter++) {
                Event e = *iter;
                if (e.Active) {
                    e.Target->Run(filter, args);
                }
            }
        }
        
        void Emit(std::string evnt, EventArgs args) {
            Emit(evnt, [](EventArgs e) { return true; }, args);
        }
        
        void Emit(std::string evnt) {
            Emit(evnt, [](EventArgs e) { return true; }, EventArgs());
        }
        
        int On(std::string evnt, std::string name, EventArgs e, EventTargetFunc target) {
            _events.push_back(Event(std::string(evnt.c_str()), std::string(evnt.c_str()),
                                    new CPPEventTarget(target, e)));
            return lastEventID++;
        }
        
        int On(std::string evnt, std::string name, EventArgs e, v8::Persistent<v8::Function>* target) {
            _events.push_back(Event(std::string(evnt.c_str()), std::string(name.c_str()),
                                    new JSEventTarget(target, e)));
            return lastEventID++;
        }
        
        int On(std::string evnt, std::string name, EventTargetFunc target) {
            return On(evnt, name, EventArgs(), target);
        }
        
        int On(std::string evnt, std::string name, v8::Persistent<v8::Function>* target) {
            return On(evnt, name, EventArgs(), target);
        }
        
        void Clear(std::string eventID) {
            for (auto iter = _events.begin(); iter != _events.end(); iter++) {
                if (iter->Label == eventID) {
                    iter->Active = false;
                }
            }
        }
    }
}