#include "Events.hpp"

#include "main.hpp"

namespace Engine {
    namespace Events {
        
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
                
                args[0] = e.getObject();
                
                v8::Local<v8::Function> func = v8::Local<v8::Function>::New(v8::Isolate::GetCurrent(), *_func);
                
                func->Call(ctx->Global(), 1, args);
            
                if (!tryCatch.StackTrace().IsEmpty()) {
                    v8::Handle<v8::Value> exception = tryCatch.StackTrace();
                    v8::String::AsciiValue exception_str(exception);
                    Logger::begin("Scripting", Logger::LogLevel_Error) << "Exception in C++ to JS Call: " << *exception_str << Logger::end();
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
            
            return ret;
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
        
        bool eventExists(std::string name) {
            for (auto iter = _events.begin(); iter != _events.end(); iter++) {
                if (iter->Label == name) { // I should switch this over to a unordered_map
                    return true;
                }
            }
            return false;
        }
        
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
            if (eventExists(name)) {
                return -1;
            } else {
                _events.push_back(Event(std::string(evnt.c_str()), std::string(evnt.c_str()),
                                        new CPPEventTarget(target, e)));
                return lastEventID++;
            }
        }
        
        int On(std::string evnt, std::string name, EventArgs e, v8::Persistent<v8::Function>* target) {
            if (eventExists(name)) {
                return -1;
            } else {
                _events.push_back(Event(std::string(evnt.c_str()), std::string(name.c_str()),
                                        new JSEventTarget(target, e)));
                return lastEventID++;
            }
        }
        
        int On(std::string evnt, std::string name, EventTargetFunc target) {
            return On(evnt, name, EventArgs(), target);
        }
        
        int On(std::string evnt, std::string name, v8::Persistent<v8::Function>* target) {
            return On(evnt, name, EventArgs(), target);
        }
        
        void Clear(int eventID) {
            if (eventID < _events.size()) {
                _events.at(eventID).Active = false;
            }
        }
    }
}