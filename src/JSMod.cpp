/*
   Filename: JSMod.cpp
   Purpose:  Javascript bindings for Engine::Platform::Libary

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

#include "JSMod.hpp"

#include <map>

#include "common.hpp"
#include "Filesystem.hpp"

namespace Engine {
    
	namespace JSMod {
        
        int _lastOpenModule = 0;
        std::map<int, Platform::Libary*> _openModules;

        ENGINE_JS_METHOD(OpenModule) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the path of the module to load");
            
            std::string moduleName = ENGINE_GET_ARG_CPPSTRING_VALUE(0) + _PLATFORM_DYLINK;
            
            if (!Filesystem::FileExists(moduleName)) {
                ENGINE_THROW_ARGERROR("module does not exist for this platform");
            }
            
            Platform::Libary* lib = Platform::OpenLibary(Filesystem::GetRealPath(moduleName));
            
            if (!lib->IsValid()) {
                ENGINE_THROW_ARGERROR("could not load module");
            }
            
            int modID = _lastOpenModule;
            _openModules[_lastOpenModule++] = lib;

            ENGINE_JS_SCOPE_CLOSE(v8::Number::New(modID));
        }
        
        ENGINE_JS_METHOD(CallMethod) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_INT32(0, "Arg0 is the handle to the module returned from mod.open");
            ENGINE_CHECK_ARG_STRING(1, "Arg1 is the name of the method to call");
            
            _openModules[ENGINE_GET_ARG_INT32_VALUE(0)]->CallMethod(ENGINE_GET_ARG_CPPSTRING_VALUE(1));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
#define addItem(table, js_name, funct) table->Set(js_name, v8::FunctionTemplate::New(funct))
        
        void InitMod(v8::Handle<v8::ObjectTemplate> modTable) {
            addItem(modTable, "open", OpenModule);
            addItem(modTable, "call", CallMethod);
        }
        
#undef addItem
        
        void CloseAllOpenModules() {
            for (auto iter = _openModules.begin(); iter != _openModules.end(); iter++) {
                if (iter->second->IsValid()) {
                    delete iter->second;
                }
            }
        }

    }
}