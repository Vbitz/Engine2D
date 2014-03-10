/*
   Filename: Events.cpp
   Purpose:  Mutli-targeting inner process message passing

   Part of Engine2D

   Copyright (C) 2014 Vbitz

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "Events.hpp"

#include <vector>
#include <unordered_map>

#include "Logger.hpp"

namespace Engine {
    namespace Events {
        
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
            
            bool IsScript() override { return false; }
            
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
            JSEventTarget(v8::Handle<v8::Function> func, Json::Value* filter){
                _func.Reset(v8::Isolate::GetCurrent(), func);
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
                
                v8::Local<v8::Function> func = v8::Local<v8::Function>::New(v8::Isolate::GetCurrent(), _func);
                
                func->Call(ctx->Global(), 1, args);
            
                if (!tryCatch.StackTrace().IsEmpty()) {
                    ScriptingManager::ReportException(v8::Isolate::GetCurrent(), &tryCatch);
                    return false;
                } else {
                    return true;
                }
            }
            
        private:
            v8::Persistent<v8::Function> _func;
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
            
            std::string ToString() {
                std::stringstream ss;
                ss << "NoScript=" << this->NoScript;
                return ss.str();
            }
        };
        
        struct EventClass {
            bool Valid = false;
            std::string TargetName;
            EventClassSecurity Security;
            std::unordered_map<std::string, Event> Events;
        };
        
        std::unordered_map<std::string, EventClass> _events;
        
        int lastEventID = 0;
        
        void _debug(Json::Value args) {
            Logger::begin("Events", Logger::LogLevel_Log) << " == EVENT DEBUG ==" << Logger::end();
            
            for (auto iter = _events.begin();
                 iter != _events.end();
                 iter++) {
                Logger::begin("Events", Logger::LogLevel_Log)
                    << "    ==========================="
                    << Logger::end();
                Logger::begin("Events", Logger::LogLevel_Log)
                    << "    Event Class: " << iter->first
                    << Logger::end();
                Logger::begin("Events", Logger::LogLevel_Log)
                    << "    Event Security: " << iter->second.Security.ToString()
                    << Logger::end();
                Logger::begin("Events", Logger::LogLevel_Log)
                    << "    Event Members: "
                    << Logger::end();
                
                for (auto iter2 = iter->second.Events.begin();
                     iter2 != iter->second.Events.end();
                     iter2++) {
                    Logger::begin("Events", Logger::LogLevel_Log)
                    << "        " << (iter2->second.Target->IsScript() ? "Script" : "C++   ") << " | " << iter2->first
                        << Logger::end();
                }
            }
        }
        
        void Init() {
            Events::On("eventDebug", "Events::_debug", _debug);
        }
        
        void Emit(std::string evnt, std::function<bool(Json::Value)> filter, Json::Value args) {
            std::vector<Event> deleteTargets;
            EventClass& cls = _events[evnt];
            if (!cls.Valid) return;
            for (auto iter = cls.Events.begin(); iter != cls.Events.end(); iter++) {
                if (iter->second.Target == NULL) { throw "Invalid Target"; }
                if (iter->second.Active) {
                    if (!(cls.Security.NoScript && iter->second.Target->IsScript())) {
                        iter->second.Target->Run(filter, args);
                    }
                } else {
                    deleteTargets.push_back(iter->second);
                }
            }
            for (auto iter = deleteTargets.begin(); iter != deleteTargets.end(); iter++) {
                delete cls.Events[iter->Label].Target;
                cls.Events.erase(iter->Label);
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
        
        void On(std::string evnt, std::string name, Json::Value e, v8::Handle<v8::
            Function> target) {
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
        
        void On(std::string evnt, std::string name, v8::Handle<v8::Function> target) {
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