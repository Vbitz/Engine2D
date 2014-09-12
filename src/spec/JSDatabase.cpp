/*
   Filename: JSDatabase.cpp
   Purpose:  Javascript bindings for Engine::Database

   Part of Engine2D

   Copyright (C) 2014 Vbitz

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "../JSDatabase.hpp"

#include "../Database.hpp"
#include "../Util.hpp"
#include "../ScriptingManager.hpp"
#include "../Filesystem.hpp"

namespace Engine {
    namespace JSDatabase {
        DatabasePtr currentDatabase = NULL;
        
        void OpenDatabase(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the filename of the database")) return;
            
            if (args.Assert(Filesystem::HasSetUserDir(), "fs.configDir needs to be called before db.open can be called")) return;
            
            std::string path = args.StringValue(0);
            
            Filesystem::TouchFile(path);
            
            if (currentDatabase != NULL) {
                delete currentDatabase;
                currentDatabase = NULL;
            }
            
            currentDatabase = Database::CreateDatabase(Filesystem::GetRealPath(path));
        }
        
        void CloseDatabase(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(currentDatabase != NULL, "A database needs to be loaded with fs.open before it can be closed")) return;
            
            delete currentDatabase;
            currentDatabase = NULL;
        }
        
        void Exec(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the query to be run on the database")) return;
            
            if (args.Assert(currentDatabase != NULL, "A database needs to be loaded with fs.open before it can be closed")) return;
            
            currentDatabase->Execute(args.StringValue(0));
        }
        
        void ExecPrepare(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            v8::Handle<v8::Number> num = v8::Number::New(args.GetIsolate(), 2);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the query to be run on the database")) return;
            
            if (args.Assert(currentDatabase != NULL, "A database needs to be loaded with fs.open before it can be closed")) return;
            
            v8::Handle<v8::Array> arr = v8::Array::New(args.GetIsolate());
            
            std::vector<std::map<std::string, std::string>> ret = currentDatabase->ExecutePrepare(args.StringValue(0));
            
            int rows = 0;
            
            for (auto iterator1 = ret.begin(); iterator1 != ret.end(); iterator1++) {
                v8::Local<v8::Object> row = args.NewObject();
                for (auto iterator2 = iterator1->begin(); iterator2 != iterator1->end(); iterator2++) {
                    row->Set(args.NewString(iterator2->first), args.NewString(iterator2->second));
                }
                arr->Set(rows++, row);
            }
            
            args.SetReturnValue(arr);
        }
        
        void InitDatabase(v8::Handle<v8::ObjectTemplate> dbTable) {
            ScriptingManager::Factory f(v8::Isolate::GetCurrent());
            
            f.FillTemplate(dbTable, {
                {FTT_Static, "open", f.NewFunctionTemplate(OpenDatabase)},
                {FTT_Static, "close", f.NewFunctionTemplate(CloseDatabase)},
                {FTT_Static, "exec", f.NewFunctionTemplate(Exec)},
                {FTT_Static, "execPrepare", f.NewFunctionTemplate(ExecPrepare)}
            });
        }
    }
}