#pragma once

#include "Scripting.hpp"
#include "vendor/json/json.h"

namespace Engine {
    namespace ScriptingManager {
        Json::Value ObjectToJson(v8::Handle<v8::Object> obj);
        v8::Handle<v8::Object> GetObjectFromJson(Json::Value val);
    }
}