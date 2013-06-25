#pragma once

#include "Filesystem.hpp"
#include "Database.hpp"

#include "common.hpp"

namespace Engine {
    namespace JSDatabase {
        void InitDatabase(v8::Handle<v8::ObjectTemplate> dbTable);
    }
}