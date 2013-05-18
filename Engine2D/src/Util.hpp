#pragma once

#ifdef _WIN32
#include <windows.h>
#define sleep Sleep
#else
#include <unistd.h>
#endif

#include "common.hpp"

#define ENGINE_JS_METHOD(name) v8::Handle<v8::Value> name(const v8::Arguments& args)

#define ENGINE_CHECK_ARGS_LENGTH(num) if (args.Length() != num) {return scope.Close(v8::Undefined());}

#define ENGINE_CHECK_ARG_INT32(num) if (!args[num]->IsNumber()) {return scope.Close(v8::Undefined());}
#define ENGINE_CHECK_ARG_NUMBER(num) ENGINE_CHECK_ARG_INT32(num)
#define ENGINE_GET_ARG_INT32_VALUE(num) args[num]->Int32Value()
#define ENGINE_GET_ARG_NUMBER_VALUE(num) args[num]->NumberValue()

#define ENGINE_CHECK_ARG_FUNCTION(num) if (!args[num]->IsFunction()) {return scope.Close(v8::Undefined());}

#define ENGINE_CHECK_ARG_STRING(num) if (!args[num]->IsString()) {return scope.Close(v8::Undefined());}
#define ENGINE_GET_ARG_CSTRING_VALUE(num) v8::String::AsciiValue(args[num])
#define ENGINE_GET_ARG_CPPSTRING_VALUE(num) std::string(*v8::String::AsciiValue(args[num]))

#define ENGINE_CHECK_ARG_BOOLEAN(num) if (!args[num]->IsBoolean()) {return scope.Close(v8::Undefined());}
#define ENGINE_GET_ARG_BOOLEAN_VALUE(num) args[num]->ToBoolean()->Value()

#define ENGINE_JS_SCOPE_OPEN v8::HandleScope scope
#define ENGINE_JS_SCOPE_CLOSE(value) return scope.Close(value)
#define ENGINE_JS_SCOPE_CLOSE_UNDEFINED return scope.Close(v8::Undefined())

namespace Engine {
    inline bool CheckGLError(std::string source) {
        GLenum err;
        bool oneError = false;
        while ((err = glGetError()) != GL_NO_ERROR) {
            oneError = true;
            std::cerr << "GLError in " << source << std::endl;
            if (err == GL_INVALID_ENUM) {
                std::cerr << "INVALID ENUM!" << std::endl;
            } else if (err == GL_INVALID_VALUE) {
                std::cerr << "Invalid value!" << std::endl;
            } else if (err == GL_INVALID_OPERATION) {
                std::cerr << "Invalid operation!" << std::endl;
            } else if (err == GL_OUT_OF_MEMORY) {
                std::cerr << "Out of Memory" << std::endl;
            }
        }
        return oneError;
    }
}