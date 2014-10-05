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
        class JS_Database : public ScriptingManager::ObjectWrap {
        public:
            ~JS_Database() {
                delete this->_db;
            }

            static void Create(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                if (args.RecallAsConstructor()) return;
                
                args.AssertCount(1);
                
                args.Assert(args[0]->IsString(), "Arg0 is the Filename to load or create the database from");
                
                JS_Database::Wrap<JS_Database>(args.GetIsolate(), args.This());

                Filesystem::TouchFile(args.StringValue(0));

                JS_Database::Unwrap<JS_Database>(args.This())->_db = Database::CreateDatabase(Filesystem::GetRealPath(args.StringValue(0)));
            }

            static void Exec(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                if (args.AssertCount(1)) return;
                
                if (args.Assert(args[0]->IsString(), "Arg0 is the query to be run on the database")) return;
                
                JS_Database::Unwrap<JS_Database>(args.This())->_db->Execute(args.StringValue(0));
            }
            
            static void ExecPrepare(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                v8::Handle<v8::Number> num = v8::Number::New(args.GetIsolate(), 2);
                
                if (args.AssertCount(1)) return;
                
                if (args.Assert(args[0]->IsString(), "Arg0 is the query to be run on the database")) return;
                
                v8::Handle<v8::Array> arr = v8::Array::New(args.GetIsolate());
                
                std::vector<std::map<std::string, std::string>> ret = JS_Database::Unwrap<JS_Database>(args.This())
                    ->_db->ExecutePrepare(args.StringValue(0));
                
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

            static void Init(v8::Handle<v8::ObjectTemplate> databaseTable) {
                ScriptingManager::Factory f(v8::Isolate::GetCurrent());
                v8::HandleScope scope(f.GetIsolate());
                
                v8::Handle<v8::FunctionTemplate> newDatabase = f.NewFunctionTemplate(Create);
                
                newDatabase->SetClassName(f.NewString("Database"));
                
                f.FillTemplate(newDatabase, {
                    {FTT_Prototype, "exec", f.NewFunctionTemplate(Exec)},
                    {FTT_Prototype, "execPrepare", f.NewFunctionTemplate(ExecPrepare)}
                });
                
                newDatabase->InstanceTemplate()->SetInternalFieldCount(1);
                
                databaseTable->Set(f.GetIsolate(), "Database", newDatabase);
            }

        private:
            DatabasePtr _db =  NULL;
        };

        void InitDatabase(v8::Handle<v8::ObjectTemplate> dbTable) {
            ScriptingManager::Factory f(v8::Isolate::GetCurrent());
            
            JS_Database::Init(dbTable);
        }
    }
}