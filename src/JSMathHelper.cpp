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
#include "Util.hpp"
#include "stdlib.hpp"

#include <random>

namespace Engine {
    namespace JsMathHelper {
        
        ENGINE_JS_METHOD(Random_New) {
            ENGINE_JS_SCOPE_OPEN;
            
            BasicRandom* rand;
            
            if (args.Length() == 1) {
                rand = new BasicRandom(ENGINE_GET_ARG_NUMBER_VALUE(0));
            } else {
                rand = new BasicRandom();
            }
            
            args.This()->SetHiddenValue(v8::String::NewSymbol("__rand"), v8::External::New(rand));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(Random_Next) {
            ENGINE_JS_SCOPE_OPEN;
            
            BasicRandom* rand = (BasicRandom*) args.This()->GetHiddenValue(v8::String::NewSymbol("__rand")).As<v8::External>()->Value();
            
            if (args.Length() == 0) {
                ENGINE_JS_SCOPE_CLOSE(v8::Number::New(rand->Next()));
            } else if (args.Length() == 1) {
                ENGINE_JS_SCOPE_CLOSE(v8::Number::New(rand->Next(ENGINE_GET_ARG_INT32_VALUE(0))));
            } else if (args.Length() == 2) {
                ENGINE_JS_SCOPE_CLOSE(v8::Number::New(rand->Next(ENGINE_GET_ARG_INT32_VALUE(0), ENGINE_GET_ARG_INT32_VALUE(1))));
            }
        }
        
        void InitMathHelper() {
            v8::HandleScope scp(v8::Isolate::GetCurrent());
            v8::Local<v8::Context> ctx = v8::Isolate::GetCurrent()->GetCurrentContext();
            v8::Context::Scope ctx_scope(ctx);
            
            v8::Local<v8::Object> obj = v8::Context::GetCurrent()->Global();
            
            v8::Local<v8::Object> math_table = v8::Object::Cast(*obj->Get(v8::String::New("Math")));
            
            math_table->Set(v8::String::NewSymbol("PI"), v8::Number::New(M_PI));
            math_table->Set(v8::String::NewSymbol("PI_2"), v8::Number::New(M_PI_2));
            math_table->Set(v8::String::NewSymbol("PI_4"), v8::Number::New(M_PI_4));
            
            v8::Handle<v8::FunctionTemplate> random_template = v8::FunctionTemplate::New();
            
            random_template->SetCallHandler(Random_New);
            
            v8::Handle<v8::ObjectTemplate> random_proto = random_template->PrototypeTemplate();
            
            random_proto->Set("next", v8::FunctionTemplate::New(Random_Next));
            
            math_table->Set(v8::String::NewSymbol("Random"), random_template->GetFunction());
        }
    }
}