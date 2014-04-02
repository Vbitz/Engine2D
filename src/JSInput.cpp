/*
   Filename: JSInput.cpp
   Purpose:  Various Javascript input functions

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

#include "JSInput.hpp"

#include "Application.hpp"

namespace Engine {
    namespace JsInput {
        
        char *convertToUpper(char *str){
            char *newstr, *p;
            p = newstr = strdup(str);
            while(*p++ != 0x00) {
                *p = std::toupper(*p);
            }
            return newstr;
        }
        
        ENGINE_JS_METHOD(KeyDown) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the Key to check the status of");
            
            const char* str = convertToUpper(*ENGINE_GET_ARG_CSTRING_VALUE(0));
            
            int key = (int) str[0];
            
            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(GetAppSingilton()->GetKeyPressed(key)));
        }
        
#define addItem(table, js_name, funct) table->Set(js_name, v8::FunctionTemplate::New(funct))
        
        void InitInput(v8::Handle<v8::ObjectTemplate> inputTable) {
            addItem(inputTable, "keyDown", KeyDown);
        }
        
#undef addItem
    }
}