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

#include "../JSInput.hpp"

#include "../Platform.hpp"

#include <string.h>

#include "../Application.hpp"
#include "../Scripting.hpp"
#include "../Util.hpp"

namespace Engine {
    namespace JsInput {
        
        char *convertToUpper(const char *str){
            char *newstr, *p;
            p = newstr = strdup(str);
            while(*p++ != 0x00) {
#ifdef _PLATFORM_WIN32
				*p = toupper(*p);
#else 
				*p = std::toupper(*p);
#endif
            }
            return newstr;
        }
        
        void KeyDown(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the Key to check the status of")) return;
            
            const char* str = convertToUpper(args.CStringValue(0));
            
            int key = (int) str[0];
            
            args.SetReturnValue(args.NewBoolean(GetAppSingilton()->GetKeyPressed(key)));
        }
        
        void SetCaptureMouse(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            GetAppSingilton()->GetWindow()->SetCaptureMouse(args.BooleanValue(0));
        }
        
        void InitInput(v8::Handle<v8::ObjectTemplate> inputTable) {
            ScriptingManager::Factory f(v8::Isolate::GetCurrent());
            f.FillTemplate(inputTable, {
                {FTT_Static, "keyDown", f.NewFunctionTemplate(KeyDown)},
                {FTT_Static, "setCaptureMouse", f.NewFunctionTemplate(SetCaptureMouse)}
            });
        }
    }
}
