#include "JSDatabase.hpp"

namespace Engine {
    namespace JSDatabase {
        Database* currentDatabase = NULL;
        
        ENGINE_JS_METHOD(SetDatabaseFilename) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the filename of the database");
            
            if (!Filesystem::HasSetUserDir()) {
                ENGINE_THROW_ARGERROR("fs.configDir needs to be called before db.open can be called");
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            std::string path = ENGINE_GET_ARG_CPPSTRING_VALUE(0);
            
            Filesystem::TouchFile(path);
            
            currentDatabase = new Database(Filesystem::GetRealPath(path));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(Exec) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the query to be run on the database");
            
            currentDatabase->Execute(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(ExecPrepare) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the query to be run on the database");
            
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
        
#define addItem(table, js_name, funct) table->Set(js_name, v8::FunctionTemplate::New(funct))
        
        void InitDatabase(v8::Handle<v8::ObjectTemplate> dbTable) {
            addItem(dbTable, "open", SetDatabaseFilename);
            addItem(dbTable, "exec", Exec);
            addItem(dbTable, "execPrepare", ExecPrepare);
        }
        
#undef addItem
    }
}