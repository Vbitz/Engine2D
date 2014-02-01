#pragma once

#include "Scripting.hpp"

namespace Engine {
    namespace JSDatabase {
        void InitDatabase(v8::Handle<v8::ObjectTemplate> dbTable);
    }
}