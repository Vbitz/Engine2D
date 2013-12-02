#include "JSFS.hpp"

#include "Util.hpp"
#include "Filesystem.hpp"

namespace Engine {
    namespace JsFS {
        ENGINE_JS_METHOD(ReadFile) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the path to the file to read");
            
            std::string path = ENGINE_GET_ARG_CPPSTRING_VALUE(0);
            
            if (!Filesystem::FileExists(path)) {
                ENGINE_THROW_ARGERROR("File does not exist");
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            ENGINE_JS_SCOPE_CLOSE(v8::String::New(Filesystem::GetFileContent(path)));
        }
        
        ENGINE_JS_METHOD(WriteFile) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the path to the file to write");
            ENGINE_CHECK_ARG_STRING(1, "Arg1 is the content to write to the file");
            
            Filesystem::WriteFile(ENGINE_GET_ARG_CPPSTRING_VALUE(0), *ENGINE_GET_ARG_CSTRING_VALUE(1), args[1]->ToString()->Length());
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(FileExists) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the path to the file");
            
            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(Filesystem::FileExists(ENGINE_GET_ARG_CPPSTRING_VALUE(0))));
        }
        
        ENGINE_JS_METHOD(FileSize) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the path to the file");
            
            ENGINE_JS_SCOPE_CLOSE(v8::Integer::New((int) Filesystem::FileSize(ENGINE_GET_ARG_CPPSTRING_VALUE(0))));
        }
        
        ENGINE_JS_METHOD(MountFile) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the path to the archivefile");
            ENGINE_CHECK_ARG_STRING(1, "Arg1 is the path to mount the archive to");
            
            std::string path = ENGINE_GET_ARG_CPPSTRING_VALUE(0);
            
            if (!Filesystem::FileExists(path)) {
                ENGINE_THROW_ARGERROR("File does not exist");
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            bool result = Filesystem::Mount(Filesystem::GetRealPath(path), ENGINE_GET_ARG_CPPSTRING_VALUE(1));
            
            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(result));
        }
        
        ENGINE_JS_METHOD(ConfigDir) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the application name");
            
            Filesystem::SetupUserDir(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(Mkdir) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the path to the directory to create");
            
            Filesystem::Mkdir(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
#define addItem(table, js_name, funct) table->Set(js_name, v8::FunctionTemplate::New(funct))
        
        void InitFS(v8::Handle<v8::ObjectTemplate> fsTable) {
            addItem(fsTable, "readFile", ReadFile);
            addItem(fsTable, "writeFile", WriteFile);
            addItem(fsTable, "fileExists", FileExists);
            addItem(fsTable, "fileSize", FileSize);
            addItem(fsTable, "mountFile", MountFile);
            addItem(fsTable, "configDir", ConfigDir);
            addItem(fsTable, "mkdir", Mkdir);
        }
        
#undef addItem
        
    }
}