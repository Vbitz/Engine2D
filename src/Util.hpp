#pragma once

#include "common.hpp"

#ifdef _WIN32
#define sleep Sleep
#else
#include <unistd.h>
#endif

#define ENGINE_THROW_ARGCOUNT(count) v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong number of arguments, expected " + count)));
#define ENGINE_THROW_ARGERROR(str) v8::ThrowException(v8::Exception::TypeError(v8::String::New(str)));

#define ENGINE_JS_METHOD(name) v8::Handle<v8::Value> name(const v8::Arguments& args)

#define ENGINE_CHECK_ARGS_LENGTH(num) if (args.Length() != num) {ENGINE_THROW_ARGCOUNT(num);return scope.Close(v8::Undefined());}

#define ENGINE_CHECK_ARG_INT32(num, errStr) if (!args[num]->IsNumber()) {ENGINE_THROW_ARGERROR(errStr);return scope.Close(v8::Undefined());}
#define ENGINE_CHECK_ARG_NUMBER(num, errStr) ENGINE_CHECK_ARG_INT32(num, errStr)
#define ENGINE_GET_ARG_INT32_VALUE(num) args[num]->Int32Value()
#define ENGINE_GET_ARG_NUMBER_VALUE(num) args[num]->NumberValue()

#define ENGINE_CHECK_ARG_FUNCTION(num, errStr) if (!args[num]->IsFunction()) {ENGINE_THROW_ARGERROR(errStr);return scope.Close(v8::Undefined());}

#define ENGINE_CHECK_ARG_STRING(num, errStr) if (!args[num]->IsString()) {ENGINE_THROW_ARGERROR(errStr);return scope.Close(v8::Undefined());}
#define ENGINE_GET_ARG_CSTRING_VALUE(num) v8::String::AsciiValue(args[num])
#define ENGINE_GET_ARG_CPPSTRING_VALUE(num) std::string(*v8::String::AsciiValue(args[num]))

#define ENGINE_CHECK_ARG_BOOLEAN(num, errStr) if (!args[num]->IsBoolean()) {ENGINE_THROW_ARGERROR(errStr);return scope.Close(v8::Undefined());}
#define ENGINE_GET_ARG_BOOLEAN_VALUE(num) args[num]->ToBoolean()->Value()

#define ENGINE_CHECK_ARG_ARRAY(num, errStr) if (!args[num]->IsArray()) {ENGINE_THROW_ARGERROR(errStr);return scope.Close(v8::Undefined());}
#define ENGINE_GET_ARG_ARRAY(num) v8::Array::Cast(*args[num])

#define ENGINE_CHECK_ARG_OBJECT(num, errStr) if (!args[num]->IsObject()) {ENGINE_THROW_ARGERROR(errStr);return scope.Close(v8::Undefined());}
#define ENGINE_GET_ARG_OBJECT(num) v8::Object::Cast(*args[num])

#define ENGINE_JS_SCOPE_OPEN v8::HandleScope scope
#define ENGINE_JS_SCOPE_CLOSE(value) return scope.Close(value)
#define ENGINE_JS_SCOPE_CLOSE_UNDEFINED return scope.Close(v8::Undefined())

namespace Engine {
    inline bool CheckGLError(std::string source) {
        GLenum err;
        bool oneError = false;
        while ((err = glGetError()) != GL_NO_ERROR) {
            oneError = true;
            std::cerr << "GLError in " << source << " : ";
            if (err == GL_INVALID_ENUM) {
                std::cerr << "Invalid Enum!" << std::endl;
            } else if (err == GL_INVALID_VALUE) {
                std::cerr << "Invalid value!" << std::endl;
            } else if (err == GL_INVALID_OPERATION) {
                std::cerr << "Invalid operation!" << std::endl;
            } else if (err == GL_OUT_OF_MEMORY) {
                std::cerr << "Out of Memory!" << std::endl;
            } else {
                std::cerr << "Unknown Error: " << err << std::endl;
            }
        }
        return oneError;
    }
}