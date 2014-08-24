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

#include "vendor/glm/glm.hpp"

#include <random>
#include <iostream>
#include <sstream>

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
                    args.SetReturnValue(args.NewNumber(Unwrap<JS_BasicRandom>(args.This())->BasicRandom::Next()));
                } else if (args.Length() == 1) {
                    args.SetReturnValue(args.NewNumber(Unwrap<JS_BasicRandom>(args.This())->BasicRandom::Next(args.NumberValue(0))));
                } else if (args.Length() == 2) {
                    args.SetReturnValue(args.NewNumber(Unwrap<JS_BasicRandom>(args.This())->BasicRandom::Next(args.NumberValue(0), args.NumberValue(1))));
                }
            }
            
            static void NextDouble(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                args.SetReturnValue(args.NewNumber(Unwrap<JS_BasicRandom>(args.This())->BasicRandom::NextDouble()));
            }
            
            static void NextNormal(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                args.SetReturnValue(args.NewNumber(Unwrap<JS_BasicRandom>(args.This())->BasicRandom::NextNormal(args.NumberValue(0), args.NumberValue(1))));
            }
            
            static void CreateInterface(v8::Isolate* isolate, v8::Handle<v8::Object> math_table) {
                ScriptingManager::Factory f(isolate);
                
                v8::Handle<v8::FunctionTemplate> random_template = v8::FunctionTemplate::New(f.GetIsolate());
                
                random_template->SetCallHandler(JS_BasicRandom::New);
                
                f.FillTemplate(random_template, {
                    {FTT_Prototype, "next", f.NewFunctionTemplate(JS_BasicRandom::Next)},
                    {FTT_Prototype, "nextDouble", f.NewFunctionTemplate(JS_BasicRandom::NextDouble)},
                    {FTT_Prototype, "nextNormal", f.NewFunctionTemplate(JS_BasicRandom::NextNormal)},
                });
                
                random_template->InstanceTemplate()->SetInternalFieldCount(1);
                
                math_table->Set(f.NewString("Random"), random_template->GetFunction());
            }
        };
        
        class JS_Vector : public ScriptingManager::ObjectWrap {
        public:
            static glm::vec4 FromJSVector(ScriptingManager::Factory& fac, v8::Handle<v8::Value> thisValue) {
                v8::Handle<v8::Object> obj = v8::Handle<v8::Object>::Cast(thisValue);
                return glm::vec4(
                                 obj->Get(fac.NewString("x"))->NumberValue(),
                                 obj->Get(fac.NewString("y"))->NumberValue(),
                                 obj->Get(fac.NewString("z"))->NumberValue(),
                                 obj->Get(fac.NewString("a"))->NumberValue()
                                );
            }
            
            static v8::Handle<v8::Object> ToJSVector(ScriptingManager::Factory& fac,
                                                     v8::Handle<v8::Object> thisVec,
                                                     glm::vec4 vec) {
                v8::Handle<v8::Value> ctor = thisVec->GetConstructor();
                v8::Handle<v8::Function> ctorFunc = v8::Handle<v8::Function>::Cast(ctor);
                v8::Handle<v8::Value> instance = ctorFunc->NewInstance(0, NULL);
                assert(instance->IsObject());
                v8::Handle<v8::Object> instanceValue = instance->ToObject();
                
                instanceValue->Set(fac.NewString("x"), fac.NewNumber(vec.x));
                instanceValue->Set(fac.NewString("y"), fac.NewNumber(vec.y));
                instanceValue->Set(fac.NewString("z"), fac.NewNumber(vec.z));
                instanceValue->Set(fac.NewString("a"), fac.NewNumber(vec.a));
                
                return instanceValue;
            }
            
            static bool IsJSVector(ScriptingManager::Factory& fac, v8::Handle<v8::Value> value) {
                if (!value->IsObject()) return false;
                v8::Handle<v8::Object> obj = value->ToObject();
                
                return obj->Get(fac.NewString("x"))->IsNumber() &&
                        obj->Get(fac.NewString("y"))->IsNumber() &&
                        obj->Get(fac.NewString("z"))->IsNumber() &&
                        obj->Get(fac.NewString("a"))->IsNumber();
            }
            
            static void New(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                if (args.RecallAsConstructor()) return;
                
                JS_Vector* vec = Wrap<JS_Vector>(args.GetIsolate(), args.This());
                
                if (args.Length() > 1) {
                    args.This()->Set(args.NewString("x"), args[0]->ToNumber());
                    args.This()->Set(args.NewString("y"), args[1]->ToNumber());
                } else {
                    args.This()->Set(args.NewString("x"), args.NewNumber(0.0));
                    args.This()->Set(args.NewString("y"), args.NewNumber(0.0));
                }
                if (args.Length() > 2) {
                    args.This()->Set(args.NewString("z"), args[2]->ToNumber());
                } else {
                    args.This()->Set(args.NewString("z"), args.NewNumber(0.0));
                }
                if (args.Length() > 3) {
                    args.This()->Set(args.NewString("a"), args[3]->ToNumber());
                } else {
                    args.This()->Set(args.NewString("a"), args.NewNumber(0.0));
                }
            }
            
            static void Add(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                if (args.AssertCount(1)) return;
                
                if (args.Assert(IsJSVector(args, args[0]), "Arg1 is a Vector4")) return;
                
                args.SetReturnValue(ToJSVector(args, args.This(),
                                  FromJSVector(args, args.This()) +
                                  FromJSVector(args, args[0])));
            }
            
            static void Sub(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                if (args.AssertCount(1)) return;
                
                if (args.Assert(IsJSVector(args, args[0]), "Arg1 is a Vector4")) return;
                
                args.SetReturnValue(ToJSVector(args, args.This(),
                                               FromJSVector(args, args.This()) -
                                               FromJSVector(args, args[0])));
            }
            
            static void Dot(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
            }
            
            static void ToString(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                std::stringstream ss;
                
                ss << "[Vector (";
                
                ss << args.This()->Get(args.NewString("x"))->NumberValue() << ", ";
                ss << args.This()->Get(args.NewString("y"))->NumberValue() << ", ";
                ss << args.This()->Get(args.NewString("z"))->NumberValue() << ", ";
                ss << args.This()->Get(args.NewString("a"))->NumberValue();
                
                ss << ")]";
                
                args.SetReturnValue(args.NewString(ss.str()));
            }
            
            static void CreateInterface(v8::Isolate* isolate, v8::Handle<v8::Object>math_table) {
                ScriptingManager::Factory f(isolate);
                
                v8::Handle<v8::FunctionTemplate> vector_template = v8::FunctionTemplate::New(f.GetIsolate());
                
                vector_template->SetCallHandler(JS_Vector::New);
                
                f.FillTemplate(vector_template, {
                    {FTT_Prototype, "toString", f.NewFunctionTemplate(ToString)},
                    {FTT_Prototype, "add", f.NewFunctionTemplate(Add)},
                    {FTT_Prototype, "sub", f.NewFunctionTemplate(Sub)}
                    // TODO: dot()
                    // TODO: cross()
                });
                
                vector_template->InstanceTemplate()->SetInternalFieldCount(1);
                
                vector_template->SetClassName(f.NewString("Vector"));
                
                math_table->Set(f.NewString("Vector"), vector_template->GetFunction());
            }
        };
        
        class JS_Matrix : public ScriptingManager::ObjectWrap {
        public:
            static void New(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);

                if (args.RecallAsConstructor()) return;
                
                JS_Matrix* matrix = Wrap<JS_Matrix>(args.GetIsolate(), args.This());
            }

            static void CreateLookAt(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                v8::Handle<v8::Function> thisCtor = v8::Handle<v8::Function>::Cast(args.This());
                
                //v8::Handle<v8::Object> newInstance = thisCtor->Call(args.This(), 0, NULL);
                
                //JS_Matrix* newInstanceValue = Unwarp<JS_Matrix>(newInstance);
                
            }
                                     
            static void CreateInterface(v8::Isolate* isolate, v8::Handle<v8::Object>math_table) {
                ScriptingManager::Factory f(isolate);
                
                v8::Handle<v8::FunctionTemplate> matrix_template = v8::FunctionTemplate::New(f.GetIsolate());
                
                matrix_template->SetCallHandler(JS_Matrix::New);
                
                f.FillTemplate(matrix_template, {
                    // TODO: createLookAt
                    // TODO: add
                    // TODO: sub
                    // TODO: mul
                    // TODO: div
                    // TODO: translate
                    // TODO: scale
                    // TODO: rotate
                });
                
                matrix_template->InstanceTemplate()->SetInternalFieldCount(1);
                
                matrix_template->SetClassName(f.NewString("Matrix"));
                
                math_table->Set(f.NewString("Matrix"), matrix_template->GetFunction());
            }
            
        private:
            glm::mat4 _value;
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
            JS_Vector::CreateInterface(f.GetIsolate(), math_table);
            JS_Matrix::CreateInterface(f.GetIsolate(), math_table);
        }
    }
}