#include "JSDatabase.hpp"

namespace Engine {
    namespace JSDatabase {
        Database* currentDatabase = NULL;
        
        ENGINE_JS_METHOD(SetDatabaseFilename) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0);
            
            if (!Filesystem::HasSetUserDir()) {
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            std::string path = Filesystem::GetRealPath(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            
            std::cout << path << std::endl;
            
            currentDatabase = new Database(path);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(Exec) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0);
            
            currentDatabase->Execute(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(ExecPrepare) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0);
            
            v8::Local<v8::Array> arr = v8::Array::New();
            
            std::vector< std::map<std::string, std::string> > ret = currentDatabase->ExecutePrepare(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            
            typedef std::vector< std::map<std::string, std::string> >::iterator it_type1;
            typedef std::map<std::string, std::string>::iterator it_type2;
            
            int rows = 0;
            
            for (it_type1 iterator1 = ret.begin(); iterator1 != ret.end(); iterator1++) {
                v8::Local<v8::Object> row = v8::Object::New();
                for (it_type2 iterator2 = iterator1->begin(); iterator2 != iterator1->end(); iterator2++) {
                    row->Set(v8::String::New(iterator2->first.c_str()), v8::String::New(iterator2->second.c_str()));
                }
                arr->Set(rows++, row);
            }
            
            ENGINE_JS_SCOPE_CLOSE(arr);
        }
    }
}