#pragma once

#include <string>
#include <functional>
#include <stdarg.h>

#include "vendor/json/json.h"

#include "ScriptingManager.hpp"

namespace Engine {
    namespace Events {
        typedef void (*EventTargetFunc)(Json::Value e);
        
        void Init();
        void Emit(std::string evnt, std::function<bool(Json::Value)> filter, Json::Value args);
        void Emit(std::string evnt, Json::Value args);
        void Emit(std::string evnt);
        void On(std::string evnt, std::string label, Json::Value e, EventTargetFunc target);
        void On(std::string evnt, std::string label, Json::Value e, v8::Persistent<v8::Function>* target);
        void On(std::string evnt, std::string label, EventTargetFunc target);
        void On(std::string evnt, std::string label, v8::Persistent<v8::Function>* target);
        void Clear(std::string eventID);
    }
}