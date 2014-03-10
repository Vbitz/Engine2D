/*
   Filename: JSMathHelper.cpp
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

#include "JSMathHelper.hpp"

#include <math.h>
#include "Scripting.hpp"

namespace Engine {
    namespace JsMathHelper {
        void InitMathHelper() {
            v8::HandleScope scp(v8::Isolate::GetCurrent());
            v8::Local<v8::Context> ctx = v8::Isolate::GetCurrent()->GetCurrentContext();
            v8::Context::Scope ctx_scope(ctx);
            
            v8::Local<v8::Object> obj = v8::Context::GetCurrent()->Global();
            
            v8::Local<v8::Object> math_table = v8::Object::Cast(*obj->Get(v8::String::New("Math")));
            
            math_table->Set(v8::String::NewSymbol("PI"), v8::Number::New(M_PI));
            math_table->Set(v8::String::NewSymbol("PI_2"), v8::Number::New(M_PI_2));
            math_table->Set(v8::String::NewSymbol("PI_4"), v8::Number::New(M_PI_4));
        }
    }
}