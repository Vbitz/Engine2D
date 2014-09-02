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
#include "Profiler.hpp"

namespace Engine {
    static std::function<bool(Json::Value)> emptyFilter = [](Json::Value e) { return true; };
        
    class CPPEventTarget : public EventTarget {
    public:
        CPPEventTarget(EventTargetFunc func, void* userPointer)
            : _func(func), _userPointer(userPointer) { }
    
        EventMagic Run(Json::Value& e) override {
            if (e.isNull()) {
                return this->_func(Json::nullValue, this->_userPointer);
            } else {
                return this->_func(e, this->_userPointer);
            }
        }
            
        bool IsScript() override { return false; }
        Type GetType() override { return Type::CPlusPlus; }
            
    private:
        EventTargetFunc _func;
        void* _userPointer;
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
            return this->Run(e, 0, {});
        }
            
        Type GetType() override { return Type::Javascript; }
            
    private:
        v8::Persistent<v8::Function> _func;
    };
        
    size_t EventClass::GetDeferedMessageCount() {
        return this->_deferedMessages.size();
    }
    
    void EventClass::LogEvents(std::string logName) {
        for (auto iter2 = this->_events.begin();
             iter2 != this->_events.end();
             iter2++) {
            Logger::begin(logName, Logger::LogLevel_Log)
                << "        | " << iter2->first << " | " << (iter2->second.Target->IsScript() ? "Script" : "C++   ") << " | " << iter2->second.Label
                << Logger::end();
        }
    }
        
    EventMagic EventClass::Emit(Json::Value args, int jsArgC, v8::Handle<v8::Value> jsArgV[]) {
        ENGINE_PROFILER_SCOPE_EX(this->TargetName.c_str());
        static std::vector<int> deleteTargets;
        if (this->_alwaysDefered) {
            this->_deferedMessages.push(args);
            return EM_DEFERED;
        } else {
            int index = 0;
            bool canceled = false;
            for (auto iter = this->_events.begin(); iter != this->_events.end(); iter++) {
                if (iter->second.Target != NULL && iter->second.Active) {
                    if (!(this->Security.NoScript && iter->second.Target->IsScript())) {
                        ENGINE_PROFILER_SCOPE_EX(iter->second.Label.c_str());
                        EventMagic ret = EM_BADTARGET;
                        if (jsArgC > 0 && iter->second.Target->GetType() == EventTarget::Type::Javascript) {
                            JSEventTarget* target = (JSEventTarget*) iter->second.Target;
                            ret = target->Run(args, jsArgC, jsArgV);
                        } else {
                            ret = iter->second.Target->Run(args);
                        }
                        if (ret == EM_CANCEL) {
                            canceled = true;
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
                    this->_events.erase(*iter);
                }
                deleteTargets.empty();
            }
            return canceled ? EM_CANCEL : EM_OK;
        }
    }
        
    EventMagic EventClass::Emit(Json::Value args) {
        return this->Emit(args, 0, {});
    }
        
    EventMagic EventClass::Emit() {
        return this->Emit(Json::nullValue);
    }
        
    EventClassPtr EventClass::AddListener(size_t priority, std::string name, EventTargetPtr target) {
        for (auto iter = this->_events.begin(); iter != this->_events.end(); iter++) {
            if (iter->second.Label == name) {
                delete iter->second.Target;
                iter->second.Target = target;
                return this;
            }
        }
        this->_events.insert(std::pair<size_t, Event>(priority, Event(name, target)));
        return this;
    }
    
    EventClassPtr EventClass::AddListener(std::string name, EventTargetPtr target) {
        return this->AddListener(500, name, target);
    }
    
    void EventClass::Clear(std::string eventID) {
        for (auto iter = this->_events.begin(); iter != this->_events.end(); iter++) {
            if (iter->second.Label == eventID) {
                iter->second.Active = false;
                return;
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
        ENGINE_PROFILER_SCOPE;
        while (this->_deferedMessages.size() > 0) {
            for (auto iter2 = this->_events.begin(); iter2 != this->_events.end(); iter2++) {
                if (iter2->second.Target == NULL) { throw "Invalid Target"; }
                if (iter2->second.Active) {
                    if (!(this->Security.NoScript && iter2->second.Target->IsScript())) {
                        iter2->second.Target->Run(this->_deferedMessages.front());
                    }
                }
            }
            this->_deferedMessages.pop();
        }
    }
    
    void EventClass::AddDeferedMessage(Json::Value e) {
        this->_deferedMessages.push(e);
    }
    
    int EventClass::ListenerCount() {
        return this->_events.size();
    }
    
    EventMagic EventEmitter::_debug(Json::Value args, void* userPointer) {
        Logger::begin("Events", Logger::LogLevel_Log) << " == EVENT DEBUG ==" << Logger::end();
        
        EventEmitterPtr events = static_cast<EventEmitterPtr>(userPointer);
        
        for (auto iter = events->_events.begin();
             iter != events->_events.end();
             iter++) {
            EventClassPtr cls = iter->second;
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
    
    EventEmitter::EventEmitter() {
        this->_eventMutex = Platform::CreateMutex();
        this->GetEvent("eventDebug")->AddListener("Events::_debug", MakeTarget(_debug, this));
    }
    
    EventClassPtrRef EventEmitter::GetEvent(std::string eventName) {
        std::string evnt_copy = std::string(eventName.c_str());
        EventClassPtrRef cls = this->_events[evnt_copy];
        if (cls == NULL) {
            cls = new EventClass();
            cls->TargetName = evnt_copy;
        }
        return cls;
    }
    
    EventTarget* EventEmitter::MakeTarget(EventTargetFunc target) {
        return new CPPEventTarget(target, NULL);
    }
    
    EventTargetPtr EventEmitter::MakeTarget(EventTargetFunc target, void* userPointer) {
        return new CPPEventTarget(target, userPointer);
    }
    
    EventTargetPtr EventEmitter::MakeTarget(v8::Handle<v8::Function> target) {
        return new JSEventTarget(target);
    }
    
    void EventEmitter::Clear(std::string eventID) {
        for (auto iter = this->_events.begin();
             iter != this->_events.end(); iter++) {
            if (iter->second == NULL) return;
            iter->second->Clear(eventID);
        }
    }
    
    // Only called from the main thread
    void EventEmitter::PollDeferedMessages() {
        this->_eventMutex->Enter();
        
        for (auto iter = this->_events.begin();
             iter != this->_events.end(); iter++) {
            if (iter->second == NULL) continue;
            iter->second->PollDeferedMessages();
        }
        
        this->_eventMutex->Exit();
    }
    
    // Only called from main thread, does not interact with other threads right now
    void EventEmitter::PollDeferedMessages(std::string eventName) {
        EventClassPtrRef cls = this->_events[eventName];
        if (cls == NULL) { return; }
        
        cls->PollDeferedMessages();
    }
    
    // Called from any worker thread
    void EventEmitter::EmitThread(std::string threadID, std::string evnt, Json::Value e) {
        this->_eventMutex->Enter();
        
        if (this->_events.count(evnt) > 0) {
            this->_events[evnt]->AddDeferedMessage(e);
        }
        
        this->_eventMutex->Exit();
    }
    
    EventEmitterPtr GetEventsSingilton() {
        static EventEmitterPtr events = NULL;
        if (events == NULL) {
            events = new EventEmitter();
        }
        return events;
    }
}
