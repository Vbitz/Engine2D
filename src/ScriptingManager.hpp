#pragma once

#include "Scripting.hpp"
#include "vendor/json/json.h"

namespace Engine {
    namespace ScriptingManager {
        class ScriptingContext {
        public:
            ScriptingContext();
            ~ScriptingContext();
        };
        
        ScriptingContext* CreateScriptingContext(std::string providorName);
        
        Json::Value ObjectToJson(v8::Handle<v8::Object> obj);
        v8::Handle<v8::Object> GetObjectFromJson(Json::Value val);
    }
}