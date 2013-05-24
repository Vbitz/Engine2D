#include "JSInput.hpp"

namespace Engine {
    namespace JsInput {
        
        ENGINE_JS_METHOD(KeyDown) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0);
            
            const char* str = *ENGINE_GET_ARG_CSTRING_VALUE(0);
            
            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(glfwGetKey((int) str[0]) == GLFW_PRESS));
        }
    }
}