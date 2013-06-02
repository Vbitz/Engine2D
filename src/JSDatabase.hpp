#pragma once

#include "common.hpp"
#include "Filesystem.hpp"
#include "Database.hpp"

namespace Engine {
    namespace JSDatabase {
        void InitDatabase(v8::Handle<v8::ObjectTemplate> dbTable);
    }
}