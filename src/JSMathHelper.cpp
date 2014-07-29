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
#include "ScriptingManager.hpp"
#include "stdlib.hpp"

#include <random>
#include <iostream>

namespace Engine {
    namespace JsMathHelper {
        
        class JS_BasicRandom : public BasicRandom, public ScriptingManager::ObjectWrap {
        public:
            
            static void New(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                if (args.RecallAsConstructor()) return;
                
                JS_BasicRandom* rand = Wrap<JS_BasicRandom>(args.GetIsolate(), args.This());
                
                if (args.Length() == 1 && args[0]->IsNumber()) {
                    rand->SetSeed(args.NumberValue(0));
                }
            }
            
            static void Next(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                if (args.Length() == 0) {
                    args.SetReturnValue(args.NewNumber(Unwarp<JS_BasicRandom>(args.This())->BasicRandom::Next()));
                } else if (args.Length() == 1) {
                    args.SetReturnValue(args.NewNumber(Unwarp<JS_BasicRandom>(args.This())->BasicRandom::Next(args.NumberValue(0))));
                } else if (args.Length() == 2) {
                    args.SetReturnValue(args.NewNumber(Unwarp<JS_BasicRandom>(args.This())->BasicRandom::Next(args.NumberValue(0), args.NumberValue(1))));
                }
            }
            
            static void NextDouble(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                args.SetReturnValue(args.NewNumber(Unwarp<JS_BasicRandom>(args.This())->BasicRandom::NextDouble()));
            }
            
            static void NextNormal(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                args.SetReturnValue(args.NewNumber(Unwarp<JS_BasicRandom>(args.This())->BasicRandom::NextNormal(args.NumberValue(0), args.NumberValue(1))));
            }
            
            static void CreateInterface(v8::Isolate* isolate, v8::Handle<v8::Object> math_table) {
                v8::Handle<v8::FunctionTemplate> random_template = v8::FunctionTemplate::New(isolate);
                
                random_template->SetCallHandler(JS_BasicRandom::New);
                
                v8::Handle<v8::ObjectTemplate> random_proto = random_template->PrototypeTemplate();
                
                random_proto->Set(isolate, "next", v8::FunctionTemplate::New(isolate, JS_BasicRandom::Next));
                random_proto->Set(isolate, "nextDouble", v8::FunctionTemplate::New(isolate, JS_BasicRandom::NextDouble));
                random_proto->Set(isolate, "nextNormal", v8::FunctionTemplate::New(isolate, JS_BasicRandom::NextNormal));
                
                random_template->InstanceTemplate()->SetInternalFieldCount(1);
                
                math_table->Set(v8::String::NewFromUtf8(isolate, "Random"), random_template->GetFunction());
            }
        };
        
        void InitMathHelper() {
            ScriptingManager::Factory f(v8::Isolate::GetCurrent());
            v8::Local<v8::Context> ctx = f.GetIsolate()->GetCurrentContext();
            v8::Context::Scope ctx_scope(ctx);
            
            v8::Local<v8::Object> obj = ctx->Global();
            
            v8::Local<v8::Object> math_table = obj->Get(f.NewString("Math")).As<v8::Object>();
            
            math_table->Set(f.NewString("PI"), f.NewNumber(M_PI));
            math_table->Set(f.NewString("PI_2"), f.NewNumber(M_PI_2));
            math_table->Set(f.NewString("PI_4"), f.NewNumber(M_PI_4));
            
            JS_BasicRandom::CreateInterface(f.GetIsolate(), math_table);
        }
    }
}