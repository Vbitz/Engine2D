#pragma once

#include "common.hpp"
#include "Main.hpp"

namespace Engine {
    namespace JsFS {
        ENGINE_JS_METHOD(ReadFile);
        ENGINE_JS_METHOD(WriteFile);
        
        ENGINE_JS_METHOD(FileExists);
        ENGINE_JS_METHOD(FileSize);
        
        ENGINE_JS_METHOD(MountFile);
        
        ENGINE_JS_METHOD(ConfigDir);
        ENGINE_JS_METHOD(Mkdir);
    }
}