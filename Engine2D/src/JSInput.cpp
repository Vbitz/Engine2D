#include "JSInput.hpp"

namespace Engine {
    namespace JsInput {
        
        v8::Handle<v8::Value> KeyDown(const v8::Arguments& args) {
            v8::HandleScope handle_scope;
            
            if (args.Length() != 1) {
				return handle_scope.Close(v8::Undefined());
			}
            
			if (!args[0]->IsString()) {
				return handle_scope.Close(v8::Undefined());
			}
            
            const char* str = *v8::String::AsciiValue(args[0]);
            
            std::cout << str[0] << " : " << glfwGetKey(str[0]) << std::endl;
            
            return handle_scope.Close(v8::Boolean::New(glfwGetKey((int) str[0]) == GLFW_PRESS));
        }
    }
}