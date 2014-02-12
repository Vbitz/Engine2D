#pragma once

#include <string>
#include <map>
#include <functional>
#include <stdarg.h>

#include "vendor/json/json.h"

#include "ScriptingManager.hpp"

namespace Engine {
    namespace Events {
        typedef void (*EventTargetFunc)(Json::Value e);
        
        void Emit(std::string evnt, std::function<bool(Json::Value)> filter, Json::Value args);
        void Emit(std::string evnt, Json::Value args);
        void Emit(std::string evnt);
        int On(std::string evnt, std::string label, Json::Value e, EventTargetFunc target);
        int On(std::string evnt, std::string label, Json::Value e, v8::Persistent<v8::Function>* target);
        int On(std::string evnt, std::string label, EventTargetFunc target);
        int On(std::string evnt, std::string label, v8::Persistent<v8::Function>* target);
        void Clear(std::string eventID);
    }
}