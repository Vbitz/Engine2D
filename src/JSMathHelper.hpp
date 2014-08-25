/*
   Filename: JSMathHelper.hpp
   Purpose:  Various Math constants and functions for Javascript

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

#pragma once

#include "ScriptingManager.hpp"
#include "vendor/glm/glm.hpp"

namespace Engine {
    
	namespace JsMathHelper {
        class JS_Vector : public ScriptingManager::ObjectWrap {
        public:
            static glm::vec4 FromJSVector(ScriptingManager::Factory& fac, v8::Handle<v8::Value> thisValue);
            static v8::Handle<v8::Object> ToJSVector(ScriptingManager::Factory& fac, v8::Handle<v8::Object> thisVec, glm::vec4 vec);
            static bool IsJSVector(ScriptingManager::Factory& fac, v8::Handle<v8::Value> value);
            
            static void New(const v8::FunctionCallbackInfo<v8::Value>& _args);
            static void Add(const v8::FunctionCallbackInfo<v8::Value>& _args);
            static void Sub(const v8::FunctionCallbackInfo<v8::Value>& _args);
            static void Dot(const v8::FunctionCallbackInfo<v8::Value>& _args);
            static void ToString(const v8::FunctionCallbackInfo<v8::Value>& _args);
            
            static void CreateInterface(v8::Isolate* isolate, v8::Handle<v8::Object>math_table);
        };
        
        void InitMathHelper();
	}
    
}