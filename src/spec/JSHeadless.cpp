/*
   Filename: JSHeadless.cpp
   Purpose:  Javascript functions specfic to headless mode

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

#include <string.h>

#include "../Application.hpp"
#include "../Scripting.hpp"
#include "../Util.hpp"
#include "../Platform.hpp"

namespace Engine {
    namespace JsHeadless {
        void Sleep(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;

            if (args.Assert(args[0]->IsNumber(), "Arg0 is the time to sleep in seconds")) return;
            
            Platform::NanoSleep((int) (args.NumberValue(0) * 1000000));
        }
        
        void InitHeadless(v8::Handle<v8::ObjectTemplate> headlessTable) {
            ScriptingManager::Factory f(v8::Isolate::GetCurrent());
            f.FillTemplate(headlessTable, {
                {FTT_Static, "sleep", f.NewFunctionTemplate(Sleep)}
            });
        }
    }
}
