#include "ScriptingManager.hpp"

#define ENGINE_SCRIPT_METHOD(name) 

namespace Engine {
    namespace Test {
        /***
         {
            "path": "testing",
            "flags": ["requiresGraphicsContext"],
            "returns": "int",
            "args": [
                ["int", "arg1", "Testing Arg1"],
                ["string", "arg2", "Testing Arg2"]
            ]
         }
        */
        ENGINE_SCRIPT_METHOD(Testing);
        int Testing(const ScriptingManager::Arguments& ctx, int arg1, std::string arg2) {

        }
    }
}