#pragma once

#include "common.hpp"
#include "Filesystem.hpp"
#include "Database.hpp"

namespace Engine {
    namespace JSDatabase {
        ENGINE_JS_METHOD(SetDatabaseFilename);
        ENGINE_JS_METHOD(Exec);
        ENGINE_JS_METHOD(ExecPrepare);
    }
}