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
        
        static std::function<bool(Json::Value)> emptyFilter = [](Json::Value e) { return true; };
        
        class CPPEventTarget : public EventTarget {
        public:
            CPPEventTarget(EventTargetFunc func)
                : _func(func) { }
            
            EventMagic Run(Json::Value& e) override {
                if (e.isNull()) {
                    return this->_func(Json::nullValue);
                } else {
                    return this->_func(e);
                }
            }
            
            bool IsScript() override { return false; }
            Type GetType() override { return Type::CPlusPlus; }
            
        private:
            EventTargetFunc _func;
        };
        
        inline EventMagic GetScriptingReturnType(v8::Local<v8::Value> ret) {
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
            JSEventTarget(v8::Handle<v8::Function> func) {
                _func.Reset(v8::Isolate::GetCurrent(), func);
            }
            
            inline EventMagic Run(Json::Value& e, int jsArgC, v8::Handle<v8::Value> jsArgV[]) {
                v8::Isolate* currentIsolate = v8::Isolate::GetCurrent();
                v8::Local<v8::Context> ctx = currentIsolate->GetCurrentContext();
                if (ctx.IsEmpty() || ctx->Global().IsEmpty()) return EM_BADTARGET;
                
                v8::TryCatch tryCatch;
                
                v8::Local<v8::Value>* args = new v8::Local<v8::Value>[1 + jsArgC];
                
                if ((e.isObject() || e.isArray()) &&
                    e.getMemberNames().size() == 0) {
                    args[0] = v8::Object::New(currentIsolate);
                } else if (e.isNull()) {
                    args[0] = v8::Null(currentIsolate);
                } else {
                    args[0] = ScriptingManager::GetObjectFromJson(e);
                }
                
                if (jsArgC > 0) {
                    for (int i = 0; i < jsArgC; i++) {
                        args[i + 1] = jsArgV[i];
                    }
                }
                
                v8::Local<v8::Function> func = v8::Local<v8::Function>::New(currentIsolate, _func);
                
                v8::Local<v8::Value> ret = func->Call(ctx->Global(), 1 + jsArgC, args);
                
                delete [] args;
                
                if (!tryCatch.StackTrace().IsEmpty()) {
                    ScriptingManager::ReportException(currentIsolate, &tryCatch);
                    return EM_BADTARGET;
                } else {
                    return GetScriptingReturnType(ret);
                }
            }
            
            EventMagic Run(Json::Value& e) override {
                this->Run(e, 0, {});
            }
            
            Type GetType() override { return Type::Javascript; }
            
        private:
            v8::Persistent<v8::Function> _func;
        };
        
        Platform::MutexPtr _eventMutex;
        std::unordered_map<std::string, EventClass*> _events;
        
        int lastEventID = 0;
        
        size_t EventClass::GetDeferedMessageCount() {
            return this->_deferedMessages.size();
        }
        
        void EventClass::LogEvents(std::string logName) {
            for (auto iter2 = this->_events.begin();
                 iter2 != this->_events.end();
                 iter2++) {
                Logger::begin(logName, Logger::LogLevel_Log)
                << "        " << (iter2->Target->IsScript() ? "Script" : "C++   ") << " | " << iter2->Label
                << Logger::end();
            }
        }
        
        void EventClass::Emit(Json::Value args, int jsArgC, v8::Handle<v8::Value> jsArgV[]) {
            static std::vector<int> deleteTargets;
            if (this->_alwaysDefered) {
                this->_deferedMessages.push(args);
            } else {
                int index = 0;
                for (auto iter = this->_events.begin(); iter != this->_events.end(); iter++) {
                    if (iter->Target != NULL && iter->Active) {
                        if (!(this->Security.NoScript && iter->Target->IsScript())) {
                            EventMagic ret = EM_BADTARGET;
                            if (jsArgC > 0 && iter->Target->GetType() == EventTarget::Type::Javascript) {
                                JSEventTarget* target = (JSEventTarget*) iter->Target;
                                ret = target->Run(args, jsArgC, jsArgV);
                            } else {
                                ret = iter->Target->Run(args);
                            }
                            if (ret == EM_CANCEL) {
                                break;
                            }
                        }
                    } else {
                        deleteTargets.push_back(index);
                    }
                    index++;
                }
                if (deleteTargets.size() > 0) {
                    for (auto iter = deleteTargets.begin(); iter != deleteTargets.end(); iter++) {
                        this->_events.erase(this->_events.begin() + *iter);
                    }
                    deleteTargets.empty();
                }
            }
        }
        
        void EventClass::Emit(Json::Value args) {
            this->Emit(args, 0, {});
        }
        
        void EventClass::Emit() {
            this->Emit(Json::nullValue);
        }
        
        EventClass* EventClass::AddListener(std::string name, EventTarget* target) {
            for (auto iter = this->_events.begin(); iter != this->_events.end(); iter++) {
                if (iter->Label == name) {
                    delete iter->Target;
                    iter->Target = target;
                    return;
                }
            }
            this->_events.push_back(Event(name, target));
            return this;
        }
        
        void EventClass::Clear(std::string eventID) {
            for (auto iter = this->_events.begin(); iter != this->_events.end(); iter++) {
                if (iter->Label == eventID) {
                    iter->Active = false;
                }
            }
        }
        
        EventClass* EventClass::SetDefered(bool defered) {
            this->_alwaysDefered = defered;
            return this;
        }
        
        EventClass* EventClass::SetNoScript(bool noScript) {
            this->Security.NoScript = noScript;
            return this;
        }
        
        void EventClass::PollDeferedMessages() {
            while (this->_deferedMessages.size() > 0) {
                for (auto iter2 = this->_events.begin(); iter2 != this->_events.end(); iter2++) {
                    if (iter2->Target == NULL) { throw "Invalid Target"; }
                    if (iter2->Active) {
                        if (!(this->Security.NoScript && iter2->Target->IsScript())) {
                            iter2->Target->Run(this->_deferedMessages.front());
                        }
                    }
                }
                this->_deferedMessages.pop();
            }
        }
        
        void EventClass::AddDeferedMessage(Json::Value e) {
            this->_deferedMessages.push(e);
        }
        
        
        EventMagic _debug(Json::Value args) {
            Logger::begin("Events", Logger::LogLevel_Log) << " == EVENT DEBUG ==" << Logger::end();
            
            for (auto iter = _events.begin();
                 iter != _events.end();
                 iter++) {
                EventClass* cls = iter->second;
                Logger::begin("Events", Logger::LogLevel_Log)
                    << "    ==========================="
                    << Logger::end();
                Logger::begin("Events", Logger::LogLevel_Log)
                    << "    Event Class: " << iter->first
                    << Logger::end();
                Logger::begin("Events", Logger::LogLevel_Log)
                    << "    Event Security: " << iter->second->Security.ToString()
                    << Logger::end();
                Logger::begin("Events", Logger::LogLevel_Log) << "    Event Defered Messages: " << cls->GetDeferedMessageCount() << Logger::end();
                Logger::begin("Events", Logger::LogLevel_Log)
                    << "    Event Members: "
                    << Logger::end();
                
                cls->LogEvents("Events");
            }
            return EM_OK;
        }
        
        void Init() {
            _eventMutex = Platform::CreateMutex();
            GetEvent("eventDebug")->AddListener("Events::_debug", MakeTarget(_debug));
        }
        
        EventClassPtrRef GetEvent(std::string eventName) {
            std::string evnt_copy = std::string(eventName.c_str());
            EventClassPtrRef cls = _events[evnt_copy];
            if (cls == NULL) {
                cls = new EventClass();
                cls->TargetName = evnt_copy;
            }
            return cls;
        }
        
        EventTarget* MakeTarget(EventTargetFunc target) {
            return new CPPEventTarget(target);
        }
        
        EventTarget* MakeTarget(v8::Handle<v8::Function> target) {
            return new JSEventTarget(target);
        }
        
        void Clear(std::string eventID) {
            for (auto iter = _events.begin(); iter != _events.end(); iter++) {
                if (iter->second == NULL) return;
                iter->second->Clear(eventID);
            }
        }
        
        // Only called from the main thread
        void PollDeferedMessages() {
            _eventMutex->Enter();
            
            for (auto iter = _events.begin(); iter != _events.end(); iter++) {
                if (iter->second == NULL) continue;
                iter->second->PollDeferedMessages();
            }
            
            _eventMutex->Exit();
        }
        
        // Only called from main thread, does not interact with other threads right now
        void PollDeferedMessages(std::string eventName) {
            EventClassPtrRef cls = _events[eventName];
            if (cls == NULL) { return; }
            
            cls->PollDeferedMessages();
        }
        
        // Called from any worker thread
        void EmitThread(std::string threadID, std::string evnt, Json::Value e) {
            _eventMutex->Enter();
            
            if (_events.count(evnt) > 0) {
                _events[evnt]->AddDeferedMessage(e);
            }
            
            _eventMutex->Exit();
        }
    }
}