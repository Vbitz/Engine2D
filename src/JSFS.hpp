#pragma once

#include "common.hpp"
#include "Main.hpp"

namespace Engine {
    namespace JsFS {
        void InitFS(v8::Handle<v8::ObjectTemplate> fsTable);
    }
}