/*
   Filename: Events.hpp
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

#pragma once

#include <string>
#include <sstream>
#include <stdarg.h>

#include <vector>
#include <queue>

#include "vendor/json/json.h"

#include "ScriptingManager.hpp"

#include "RenderTypes.hpp"

namespace Engine {
    namespace Events {
        typedef EventMagic (*EventTargetFunc)(Json::Value e);
        
        class EventTarget {
        public:
            enum class Type {
                Invalid,
                CPlusPlus,
                Javascript
            };
            
            virtual EventMagic Run(Json::Value& e) = 0;
            
            virtual bool IsScript() { return true; }
            virtual Type GetType() { return Type::Invalid; }
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
        
        class EventClass {
        public:
            size_t GetDeferedMessageCount();
            void LogEvents(std::string logName);
            void Emit(Json::Value args, int jsArgC, v8::Handle<v8::Value> jsArgV[]);
            void Emit(Json::Value args);
            void Emit();
            EventClass* AddListener(std::string name, EventTarget* target);
            void Clear(std::string eventID);
            EventClass* SetDefered(bool defered);
            EventClass* SetNoScript(bool noScript);
            void PollDeferedMessages();
            void AddDeferedMessage(Json::Value e);
            
            std::string TargetName;
            EventClassSecurity Security;
        private:
            bool _alwaysDefered = false;
            std::vector<Event> _events;
            std::queue<Json::Value> _deferedMessages;
        };
        
        typedef EventClass*& EventClassPtrRef;
        
        EventClassPtrRef GetEvent(std::string eventName);
        
        void Init();
        
        EventTarget* MakeTarget(EventTargetFunc target);
        EventTarget* MakeTarget(v8::Handle<v8::Function> target);
        
        void Clear(std::string eventID);
    
        void PollDeferedMessages();
        void PollDeferedMessages(std::string eventName);
        void EmitThread(std::string threadID, std::string evnt, Json::Value e);
    }
}