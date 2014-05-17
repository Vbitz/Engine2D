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
        
        void DisposeRandomCallback(const v8::WeakCallbackData<v8::External, BasicRandom*>& data) {
            delete (BasicRandom*) data.GetValue()->Value();
        }
        
        ENGINE_JS_METHOD(Random_New) {
            ENGINE_JS_SCOPE_OPEN;
            
            if (!args.IsConstructCall()) {
                ENGINE_THROW_ARGERROR("Math.Random must be called as a constrctor");
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            BasicRandom* rand;
            
            if (args.Length() == 1) {
                rand = new BasicRandom(ENGINE_GET_ARG_NUMBER_VALUE(0));
            } else {
                rand = new BasicRandom();
            }
            
            v8::Handle<v8::External> randGen = v8::External::New(args.GetIsolate(), rand);
            
            v8::Persistent<v8::External> randGenPersist;
            randGenPersist.Reset(v8::Isolate::GetCurrent(), randGen);
            randGenPersist.SetWeak<BasicRandom*>(&rand, DisposeRandomCallback);
            
            args.This()->SetHiddenValue(v8::String::NewFromUtf8(args.GetIsolate(), "__rand"), randGen);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(Random_Next) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Isolate* isolate = args.GetIsolate();
            
            BasicRandom* rand = (BasicRandom*) args.This()->GetHiddenValue(v8::String::NewFromUtf8(isolate, "__rand")).As<v8::External>()->Value();
            
            if (args.Length() == 0) {
                ENGINE_JS_SCOPE_CLOSE(v8::Number::New(isolate, rand->Next()));
            } else if (args.Length() == 1) {
                ENGINE_JS_SCOPE_CLOSE(v8::Number::New(isolate, rand->Next(ENGINE_GET_ARG_INT32_VALUE(0))));
            } else if (args.Length() == 2) {
                ENGINE_JS_SCOPE_CLOSE(v8::Number::New(isolate, rand->Next(ENGINE_GET_ARG_INT32_VALUE(0), ENGINE_GET_ARG_INT32_VALUE(1))));
            }
        }
        
        ENGINE_JS_METHOD(Random_NextDouble) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Isolate* isolate = v8::Isolate::GetCurrent();
            
            BasicRandom* rand = (BasicRandom*) args.This()->GetHiddenValue(v8::String::NewFromUtf8(isolate, "__rand")).As<v8::External>()->Value();
            
            ENGINE_JS_SCOPE_CLOSE(v8::Number::New(isolate, rand->NextDouble()));
        }
        
        ENGINE_JS_METHOD(Random_NextNormal) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Isolate* isolate = v8::Isolate::GetCurrent();
            
            BasicRandom* rand = (BasicRandom*) args.This()->GetHiddenValue(v8::String::NewFromUtf8(isolate, "__rand")).As<v8::External>()->Value();
            
            ENGINE_JS_SCOPE_CLOSE(v8::Number::New(isolate, rand->NextNormal(ENGINE_GET_ARG_NUMBER_VALUE(0), ENGINE_GET_ARG_NUMBER_VALUE(1))));
        }
        
        void InitMathHelper() {
            v8::Isolate* isolate = v8::Isolate::GetCurrent();
            v8::HandleScope scp(isolate);
            v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
            v8::Context::Scope ctx_scope(ctx);
            
            v8::Local<v8::Object> obj = ctx->Global();
            
            v8::Local<v8::Object> math_table = v8::Object::Cast(*obj->Get(v8::String::NewFromUtf8(isolate, "Math")));
            
            math_table->Set(v8::String::NewFromUtf8(isolate, "PI"), v8::Number::New(isolate, M_PI));
            math_table->Set(v8::String::NewFromUtf8(isolate, "PI_2"), v8::Number::New(isolate, M_PI_2));
            math_table->Set(v8::String::NewFromUtf8(isolate, "PI_4"), v8::Number::New(isolate, M_PI_4));
            
            v8::Handle<v8::FunctionTemplate> random_template = v8::FunctionTemplate::New(isolate);
            
            random_template->SetCallHandler(Random_New);
            
            v8::Handle<v8::ObjectTemplate> random_proto = random_template->PrototypeTemplate();
            
            random_proto->Set(isolate, "next", v8::FunctionTemplate::New(isolate, Random_Next));
            random_proto->Set(isolate, "nextDouble", v8::FunctionTemplate::New(isolate, Random_NextDouble));
            random_proto->Set(isolate, "nextNormal", v8::FunctionTemplate::New(isolate, Random_NextNormal));
            
            math_table->Set(v8::String::NewFromUtf8(isolate, "Random"), random_template->GetFunction());
        }
    }
}