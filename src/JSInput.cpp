#include "JSInput.hpp"

namespace Engine {
    namespace JsInput {
        
        char *convertToUpper(char *str){
            char *newstr, *p;
            p = newstr = strdup(str);
            while((*p++=toupper(*p)));
            return newstr;
        }
        
        ENGINE_JS_METHOD(KeyDown) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the Key to check the status of");
            
            const char* str = convertToUpper(*ENGINE_GET_ARG_CSTRING_VALUE(0));
            
            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(glfwGetKey((int) str[0]) == GLFW_PRESS));
        }
    }
}