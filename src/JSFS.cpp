#include "JSFS.hpp"

namespace Engine {
    namespace JsFS {
        ENGINE_JS_METHOD(ReadFile) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0);
            
            std::string path = ENGINE_GET_ARG_CPPSTRING_VALUE(0);
            
            if (!Filesystem::FileExists(path)) {
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            ENGINE_JS_SCOPE_CLOSE(v8::String::New(Filesystem::GetFileContent(path)));
        }
        
        ENGINE_JS_METHOD(WriteFile) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARG_STRING(0);
            ENGINE_CHECK_ARG_STRING(1);
            
            Filesystem::WriteFile(ENGINE_GET_ARG_CPPSTRING_VALUE(0), *ENGINE_GET_ARG_CSTRING_VALUE(1), args[1]->ToString()->Length());
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(FileExists) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0);
            
            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(Filesystem::FileExists(ENGINE_GET_ARG_CPPSTRING_VALUE(0))));
        }
        
        ENGINE_JS_METHOD(FileSize) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0);
            
            ENGINE_JS_SCOPE_CLOSE(v8::Integer::New(Filesystem::FileSize(ENGINE_GET_ARG_CPPSTRING_VALUE(0))));
        }
        
        ENGINE_JS_METHOD(MountFile) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_STRING(0);
            ENGINE_CHECK_ARG_STRING(1);
            
            std::string path = ENGINE_GET_ARG_CPPSTRING_VALUE(0);
            
            if (!Filesystem::FileExists(path)) {
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            bool result = Filesystem::Mount(Filesystem::GetRealPath(path), ENGINE_GET_ARG_CPPSTRING_VALUE(1));
            
            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(result));
        }
        
        ENGINE_JS_METHOD(ConfigDir) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0);
            
            Filesystem::SetupUserDir(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(Mkdir) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0);
            
            Filesystem::Mkdir(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
    }
}