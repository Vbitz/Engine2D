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
#include <queue>

#include "Logger.hpp"
#include "Platform.hpp"

namespace Engine {
    namespace Events {
        
        class EventTarget {
        public:
            EventMagic Run(std::function<bool(Json::Value)> filter, Json::Value e) {
                if (filter(_filter)) {
                    return _run(e);
                } else {
                    return EM_BADFILTER;
                }
            }
            
            virtual bool IsScript() { return true; }
            
        protected:
            Json::Value _filter = Json::nullValue;
            
            void setFilter(Json::Value filter) {
                _filter = filter;
            }
            
            virtual EventMagic _run(Json::Value e) { return EM_BADTARGET; }
        };
        
        class CPPEventTarget : public EventTarget {
        public:
            CPPEventTarget(EventTargetFunc func, Json::Value* filter)
                : _func(func) {
                    setFilter(filter);
                }
            
            bool IsScript() override { return false; }
            
        protected:
            EventMagic _run(Json::Value e) {
                return this->_func(e);
            }
            
        private:
            EventTargetFunc _func;
        };
        
        EventMagic GetScriptingReturnType(v8::Local<v8::Value> ret) {
            if (!ret->IsExternal()) {
                return EM_OK;
            } else {
                v8::Local<v8::External> exVal = ret.As<v8::External>();
                void* value = exVal->Value();
                EventMagic* magic = (EventMagic*) value;
                return *magic;
            }
        }
        
        class JSEventTarget : public EventTarget {
        public:
            JSEventTarget(v8::Handle<v8::Function> func, Json::Value* filter){
                _func.Reset(v8::Isolate::GetCurrent(), func);
                    setFilter(filter);
                }
            
        protected:
            EventMagic _run(Json::Value e) {
                v8::Isolate* currentIsolate = v8::Isolate::GetCurrent();
                v8::Local<v8::Context> ctx = currentIsolate->GetCurrentContext();
                if (ctx.IsEmpty() || ctx->Global().IsEmpty()) return EM_BADTARGET;
                
                v8::TryCatch tryCatch;
                
                v8::Local<v8::Object> obj;
                
                if ((e.isObject() || e.isArray()) &&
                    e.getMemberNames().size() == 0) {
                    obj = v8::Object::New();
                } else {
                    obj = ScriptingManager::GetObjectFromJson(e);
                }
                
                v8::Local<v8::Value> args[1] = {obj};
                
                v8::Local<v8::Function> func = v8::Local<v8::Function>::New(currentIsolate, _func);
                
                v8::Local<v8::Value> ret = func->Call(ctx->Global(), 1, args);
            
                if (!tryCatch.StackTrace().IsEmpty()) {
                    ScriptingManager::ReportException(currentIsolate, &tryCatch);
                    return EM_BADTARGET;
                } else {
                    return GetScriptingReturnType(ret);
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
            std::queue<Json::Value> DeferedMessages;
        };
        
        Platform::Mutex* _eventMutex;
        std::unordered_map<std::string, EventClass> _events;
        
        int lastEventID = 0;
        
        EventMagic _debug(Json::Value args) {
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
            return EM_OK;
        }
        
        void Init() {
            _eventMutex = Platform::CreateMutex();
            Events::On("eventDebug", "Events::_debug", _debug);
        }
        
        void Emit(std::string evnt, std::function<bool(Json::Value)> filter, Json::Value args) {
            std::vector<Event> deleteTargets;
            
            EventClass& cls = _events[evnt];
            if (!cls.Valid) { _eventMutex->Exit(); return; }
            for (auto iter = cls.Events.begin(); iter != cls.Events.end(); iter++) {
                if (iter->second.Target == NULL) { throw "Invalid Target"; }
                if (iter->second.Active) {
                    if (!(cls.Security.NoScript && iter->second.Target->IsScript())) {
                        EventMagic ret = iter->second.Target->Run(filter, args);
                        if (ret == EM_CANCEL) {
                            break;
                        }
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
        
        // Only called from the main thread
        void PollDeferedMessages() {
            _eventMutex->Enter();
            
            for (auto iter = _events.begin(); iter != _events.end(); iter++) {
                while (iter->second.DeferedMessages.size() > 0) {
                    for (auto iter2 = iter->second.Events.begin(); iter2 != iter->second.Events.end(); iter2++) {
                        if (iter2->second.Target == NULL) { throw "Invalid Target"; }
                        if (iter2->second.Active) {
                            if (!(iter->second.Security.NoScript && iter2->second.Target->IsScript())) {
                                iter2->second.Target->Run(
                                    [](Json::Value e) { return true; },
                                    iter->second.DeferedMessages.front());
                            }
                        }
                    }
                    iter->second.DeferedMessages.pop();
                }
            }
            
            _eventMutex->Exit();
        }
        
        // Called from any worker thread
        void EmitThread(std::string threadID, std::string evnt, Json::Value e) {
            _eventMutex->Enter();
            
            if (_events.count(evnt) > 0) {
                _events[evnt].DeferedMessages.push(e);
            }
            
            _eventMutex->Exit();
        }
    }
}