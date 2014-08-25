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

#define GLM_FORCE_RADIANS
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

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
        
        glm::vec4 JS_Vector::FromJSVector(ScriptingManager::Factory& fac, v8::Handle<v8::Value> thisValue) {
            v8::Handle<v8::Object> obj = v8::Handle<v8::Object>::Cast(thisValue);
            return glm::vec4(
                             obj->Get(fac.NewString("x"))->NumberValue(),
                             obj->Get(fac.NewString("y"))->NumberValue(),
                             obj->Get(fac.NewString("z"))->NumberValue(),
                             obj->Get(fac.NewString("a"))->NumberValue()
                             );
        }
        
        v8::Handle<v8::Object> JS_Vector::ToJSVector(ScriptingManager::Factory& fac,
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
        
        bool JS_Vector::IsJSVector(ScriptingManager::Factory& fac, v8::Handle<v8::Value> value){
            if (!value->IsObject()) return false;
            v8::Handle<v8::Object> obj = value->ToObject();
            
            return obj->Get(fac.NewString("x"))->IsNumber() &&
            obj->Get(fac.NewString("y"))->IsNumber() &&
            obj->Get(fac.NewString("z"))->IsNumber() &&
            obj->Get(fac.NewString("a"))->IsNumber();
        }
        
        void JS_Vector::New(const v8::FunctionCallbackInfo<v8::Value>& _args) {
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
        
        void JS_Vector::Add(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(IsJSVector(args, args[0]), "Arg1 is a Vector4")) return;
            
            args.SetReturnValue(ToJSVector(args, args.This(),
                                           FromJSVector(args, args.This()) +
                                           FromJSVector(args, args[0])));
        }
        
        void JS_Vector::Sub(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(IsJSVector(args, args[0]), "Arg1 is a Vector4")) return;
            
            args.SetReturnValue(ToJSVector(args, args.This(),
                                           FromJSVector(args, args.This()) -
                                           FromJSVector(args, args[0])));
        }
        
        void JS_Vector::Dot(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
        }
        
        void JS_Vector::ToString(const v8::FunctionCallbackInfo<v8::Value>& _args) {
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
        
        void JS_Vector::CreateInterface(v8::Isolate* isolate, v8::Handle<v8::Object>math_table) {
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
        
        class JS_Matrix : public ScriptingManager::ObjectWrap {
        public:
            static void New(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);

                if (args.RecallAsConstructor()) return;
                
                JS_Matrix* matrix = Wrap<JS_Matrix>(args.GetIsolate(), args.This());
            }

            static void CreateLookAt(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                v8::Handle<v8::Value> ctor = args.This();
                v8::Handle<v8::Function> ctorFunc = v8::Handle<v8::Function>::Cast(ctor);
                v8::Handle<v8::Value> instance = ctorFunc->NewInstance(0, NULL);
                
                v8::Handle<v8::Object> instanceValue = v8::Handle<v8::Object>::Cast(instance);
                
                JS_Matrix* newInstanceValue = Unwrap<JS_Matrix>(instanceValue);
                
                glm::vec3 lookAt = glm::vec3(0, 1, 0);
                
                newInstanceValue->_value = glm::lookAt(glm::vec3(JS_Vector::FromJSVector(args, args[0])), glm::vec3(JS_Vector::FromJSVector(args, args[1])), lookAt);
                
                args.SetReturnValue(instanceValue);
            }
            
            static void Copy(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                v8::Handle<v8::Value> ctor = args.This()->GetConstructor();
                v8::Handle<v8::Function> ctorFunc = v8::Handle<v8::Function>::Cast(ctor);
                v8::Handle<v8::Value> instance = ctorFunc->NewInstance(0, NULL);
                
                v8::Handle<v8::Object> instanceValue = v8::Handle<v8::Object>::Cast(instance);
                
                JS_Matrix* newInstanceValue = Unwrap<JS_Matrix>(instanceValue);
                
                newInstanceValue->_value = Unwrap<JS_Matrix>(args.This())->_value;
                
                args.SetReturnValue(instanceValue);
            }
            
            static void Reset(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                JS_Matrix* jsMat = Unwrap<JS_Matrix>(args.This());
                
                jsMat->_value = glm::mat4(); // reset to identify matrix
                
                args.SetReturnValue(args.This());
            }
            
            static void Translate(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                if (args.AssertCount(1)) return;
                
                if (args.Assert(JS_Vector::IsJSVector(args, args[0]), "Arg0 is the vector to translate by")) return;
                
                JS_Matrix* jsMat = Unwrap<JS_Matrix>(args.This());
                
                glm::vec4 argValue = JS_Vector::FromJSVector(args, args[0]);
                
                glm::mat4 ret = glm::translate(jsMat->_value, glm::vec3(argValue));
                
                jsMat->_value = ret;
                
                args.SetReturnValue(args.This());
            }
            
            static void Scale(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                if (args.AssertCount(1)) return;
                
                if (args.Assert(JS_Vector::IsJSVector(args, args[0]), "Arg0 is the vector to scale by")) return;
                
                JS_Matrix* jsMat = Unwrap<JS_Matrix>(args.This());
                
                glm::vec4 argValue = JS_Vector::FromJSVector(args, args[0]);
                
                glm::mat4 ret = glm::scale(jsMat->_value, glm::vec3(argValue));
                
                jsMat->_value = ret;
                
                args.SetReturnValue(args.This());
            }
            
            static void Rotate(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                if (args.AssertCount(2)) return;
                
                if (args.Assert(args[0]->IsNumber(), "Arg0 is the angle in radians to rotate by") ||
                    args.Assert(JS_Vector::IsJSVector(args, args[1]), "Arg1 is the vector to rotate by")) return;
                
                JS_Matrix* jsMat = Unwrap<JS_Matrix>(args.This());
                
                glm::vec4 argValue = JS_Vector::FromJSVector(args, args[1]);
                
                glm::mat4 ret = glm::rotate(jsMat->_value, (float) args.NumberValue(0), glm::vec3(argValue));
                
                jsMat->_value = ret;
                
                args.SetReturnValue(args.This());
            }
            
            static void ToString(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                std::stringstream ss;
                
                glm::mat4 value = Unwrap<JS_Matrix>(args.This())->_value;
                
                ss << "[Matrix4x4 (" << std::endl;
                ss << " " << value[0][0] << ", " << value[0][1] << ", " << value[0][2] << ", " << value[0][3] << "," << std::endl;
                ss << " " << value[1][0] << ", " << value[1][1] << ", " << value[1][2] << ", " << value[1][3] << "," << std::endl;
                ss << " " << value[2][0] << ", " << value[2][1] << ", " << value[2][2] << ", " << value[2][3] << "," << std::endl;
                ss << " " << value[3][0] << ", " << value[3][1] << ", " << value[3][2] << ", " << value[3][3] << "," << std::endl;
                ss << ")]";
                
                args.SetReturnValue(args.NewString(ss.str()));
            }
                                     
            static void CreateInterface(v8::Isolate* isolate, v8::Handle<v8::Object>math_table) {
                ScriptingManager::Factory f(isolate);
                
                v8::Handle<v8::FunctionTemplate> matrix_template = v8::FunctionTemplate::New(f.GetIsolate());
                
                matrix_template->SetCallHandler(JS_Matrix::New);
                
                f.FillTemplate(matrix_template, {
                    {FTT_Static, "createLookAt", f.NewFunctionTemplate(CreateLookAt)},
                    {FTT_Prototype, "copy", f.NewFunctionTemplate(Copy)},
                    {FTT_Prototype, "reset", f.NewFunctionTemplate(Reset)},
                    // TODO: add
                    // TODO: sub
                    // TODO: mul
                    // TODO: div
                    {FTT_Prototype, "translate", f.NewFunctionTemplate(Translate)},
                    {FTT_Prototype, "scale", f.NewFunctionTemplate(Scale)},
                    {FTT_Prototype, "rotate", f.NewFunctionTemplate(Rotate)},
                    {FTT_Prototype, "toString", f.NewFunctionTemplate(ToString)}
                });
                
                matrix_template->InstanceTemplate()->SetInternalFieldCount(1);
                
                matrix_template->SetClassName(f.NewString("Matrix"));
                
                math_table->Set(f.NewString("Matrix"), matrix_template->GetFunction());
            }
            
        private:
            glm::mat4 _value;
        };
        
        void DegToRad(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsNumber(), "Arg0 is the number in degrees to convert to radians")) return;
            
            args.SetReturnValue(args.NewNumber(glm::radians(args.NumberValue(0))));
        }
        
        void InitMathHelper() {
            ScriptingManager::Factory f(v8::Isolate::GetCurrent());
            v8::Local<v8::Context> ctx = f.GetIsolate()->GetCurrentContext();
            v8::Context::Scope ctx_scope(ctx);
            
            v8::Local<v8::Object> obj = ctx->Global();
            
            v8::Local<v8::Object> math_table = obj->Get(f.NewString("Math")).As<v8::Object>();
            
            f.FillObject(math_table, {
                {FTT_Static, "PI", f.NewNumber(M_PI)},
                {FTT_Static, "PI_2", f.NewNumber(M_PI_2)},
                {FTT_Static, "PI_4", f.NewNumber(M_PI_4)},
                {FTT_Static, "degToRad", f.NewFunctionTemplate(DegToRad)->GetFunction()}
            });
            
            JS_BasicRandom::CreateInterface(f.GetIsolate(), math_table);
            JS_Vector::CreateInterface(f.GetIsolate(), math_table);
            JS_Matrix::CreateInterface(f.GetIsolate(), math_table);
        }
    }
}