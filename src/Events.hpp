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
#include <functional>
#include <stdarg.h>

#include "vendor/json/json.h"

#include "ScriptingManager.hpp"

namespace Engine {
    enum EventMagic {
        EM_OK,
        EM_CANCEL,
        
        // internal values
        EM_BADFILTER,
        EM_BADTARGET
    };
    
    namespace Events {
        typedef EventMagic (*EventTargetFunc)(Json::Value e);
        
        void Init();
        void Emit(std::string evnt, std::function<bool(Json::Value)> filter, Json::Value args);
        void Emit(std::string evnt, Json::Value args);
        void Emit(std::string evnt);
        void On(std::string evnt, std::string label, Json::Value e, EventTargetFunc target);
        void On(std::string evnt, std::string label, Json::Value e, v8::Handle<v8::Function> target);
        void On(std::string evnt, std::string label, EventTargetFunc target);
        void On(std::string evnt, std::string label, v8::Handle<v8::Function> target);
        void Clear(std::string eventID);
    
        void PollDeferedMessages();
        void EmitThread(std::string threadID, std::string evnt, Json::Value e);
    }
}