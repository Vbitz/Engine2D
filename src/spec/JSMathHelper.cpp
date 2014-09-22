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

#include "../JSMathHelper.hpp"

#include <math.h>
#include "../Scripting.hpp"
#include "../Util.hpp"
#include "../ScriptingManager.hpp"
#include "../stdlib.hpp"

#define GLM_FORCE_RADIANS
#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/matrix_transform.hpp"

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
        
        static v8::Persistent<v8::FunctionTemplate> _vectorInstance;
        
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
                                          glm::vec4 vec) {
            v8::Local<v8::FunctionTemplate> ctor = v8::Local<v8::FunctionTemplate>::New(fac.GetIsolate(), _vectorInstance);
            v8::Handle<v8::Function> ctorFunc = ctor->GetFunction();
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
            
            args.SetReturnValue(ToJSVector(args,
                                           FromJSVector(args, args.This()) +
                                           FromJSVector(args, args[0])));
        }
        
        void JS_Vector::Sub(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(IsJSVector(args, args[0]), "Arg1 is a Vector4")) return;
            
            args.SetReturnValue(ToJSVector(args,
                                           FromJSVector(args, args.This()) -
                                           FromJSVector(args, args[0])));
        }
        
        void JS_Vector::Mul(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (IsJSVector(args, args[0])) {
                args.SetReturnValue(ToJSVector(args,
                                               FromJSVector(args, args.This()) *
                                               FromJSVector(args, args[0])));
            } else if (JS_Matrix::IsJSMatrix(args, args[0]->ToObject())) {
                args.SetReturnValue(ToJSVector(args,
                                               FromJSVector(args, args.This()) *
                                               JS_Matrix::GetValue(args, args[0]->ToObject())));
            } else {
                args.ThrowArgError("Arg0 is a Vector4 or a Matrix");
            }
        }
        
        void JS_Vector::Dot(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(IsJSVector(args, args[0]), "Arg1 is a Vector4")) return;
            
            args.SetReturnValue(args.NewNumber(glm::dot(FromJSVector(args, args.This()), FromJSVector(args, args[0]))));
        }
        
        void JS_Vector::Cross(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(IsJSVector(args, args[0]), "Arg1 is a Vector4")) return;
            
            glm::vec3 ret = glm::cross(glm::vec3(FromJSVector(args, args.This())),
                                       glm::vec3(FromJSVector(args, args[0])));
            
            args.SetReturnValue(ToJSVector(args, glm::vec4(ret, 0.0)));
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
                {FTT_Prototype, "sub", f.NewFunctionTemplate(Sub)},
                {FTT_Prototype, "mul", f.NewFunctionTemplate(Mul)},
                {FTT_Prototype, "dot", f.NewFunctionTemplate(Dot)},
                {FTT_Prototype, "cross", f.NewFunctionTemplate(Cross)}
                // TOOD: distince
                // TODO: length
                // TODO: normalize
            });
            
            vector_template->InstanceTemplate()->SetInternalFieldCount(1);
            
            vector_template->SetClassName(f.NewString("Vector"));
            
            _vectorInstance.Reset(f.GetIsolate(), vector_template);
            
            math_table->Set(f.NewString("Vector"), vector_template->GetFunction());
        }
        
        v8::Persistent<v8::FunctionTemplate> _matrixInstance;
        
        v8::Handle<v8::Value> JS_Matrix::NewInstance(ScriptingManager::FactoryRef fac, glm::mat4 value) {
            v8::Local<v8::FunctionTemplate> ctor = v8::Local<v8::FunctionTemplate>::New(fac.GetIsolate(), _matrixInstance);
            v8::Handle<v8::Function> ctorFunc = ctor->GetFunction();
            v8::Handle<v8::Value> instance = ctorFunc->NewInstance(0, NULL);
            assert(instance->IsObject());
            v8::Handle<v8::Object> instanceValue = instance->ToObject();
            
            Unwrap<JS_Matrix>(instanceValue)->_value = value;
            
            return instanceValue;
        }
        
        glm::mat4 JS_Matrix::GetValue(ScriptingManager::FactoryRef fac, v8::Handle<v8::Value> val) {
            v8::Handle<v8::Object> objVal = v8::Handle<v8::Object>::Cast(val);
            return Unwrap<JS_Matrix>(objVal)->_value;
        }
        
        bool JS_Matrix::IsJSMatrix(ScriptingManager::FactoryRef fac, v8::Handle<v8::Object> val) {
            return val->GetConstructorName()->Equals(fac.NewString("Matrix"));
        }
        
         void JS_Matrix::New(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);

            if (args.RecallAsConstructor()) return;
                
            JS_Matrix* matrix = Wrap<JS_Matrix>(args.GetIsolate(), args.This());
        }
        
        
        void JS_Matrix::CreateLookAt(const v8::FunctionCallbackInfo<v8::Value>& _args) {
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
        
        void JS_Matrix::Copy(const v8::FunctionCallbackInfo<v8::Value> &_args) {
            ScriptingManager::Arguments args(_args);
            
            v8::Handle<v8::Function> ctorFunc = args.GetConstructor();
            v8::Handle<v8::Value> instance = ctorFunc->NewInstance(0, NULL);
            
            v8::Handle<v8::Object> instanceValue = v8::Handle<v8::Object>::Cast(instance);
            
            JS_Matrix* newInstanceValue = Unwrap<JS_Matrix>(instanceValue);
            
            newInstanceValue->_value = Unwrap<JS_Matrix>(args.This())->_value;
            
            args.SetReturnValue(instanceValue);
        }
        
        void JS_Matrix::Reset(const v8::FunctionCallbackInfo<v8::Value> &_args) {
            ScriptingManager::Arguments args(_args);
            
            JS_Matrix* jsMat = Unwrap<JS_Matrix>(args.This());
            
            jsMat->_value = glm::mat4(); // reset to identify matrix
            
            args.SetReturnValue(args.This());
        }
        
        void JS_Matrix::Translate(const v8::FunctionCallbackInfo<v8::Value> &_args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(JS_Vector::IsJSVector(args, args[0]), "Arg0 is the vector to translate by")) return;
            
            JS_Matrix* jsMat = Unwrap<JS_Matrix>(args.This());
            
            glm::vec4 argValue = JS_Vector::FromJSVector(args, args[0]);
            
            glm::mat4 ret = glm::translate(jsMat->_value, glm::vec3(argValue));
            
            jsMat->_value = ret;
            
            args.SetReturnValue(args.This());
        }
        
        void JS_Matrix::Scale(const v8::FunctionCallbackInfo<v8::Value> &_args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(JS_Vector::IsJSVector(args, args[0]), "Arg0 is the vector to scale by")) return;
            
            JS_Matrix* jsMat = Unwrap<JS_Matrix>(args.This());
            
            glm::vec4 argValue = JS_Vector::FromJSVector(args, args[0]);
            
            glm::mat4 ret = glm::scale(jsMat->_value, glm::vec3(argValue));
            
            jsMat->_value = ret;
            
            args.SetReturnValue(args.This());
        }
        
        void JS_Matrix::Rotate(const v8::FunctionCallbackInfo<v8::Value> &_args) {
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
        
        void JS_Matrix::ToString(const v8::FunctionCallbackInfo<v8::Value> &_args) {
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
        
        void JS_Matrix::CreateInterface(v8::Isolate *isolate, v8::Handle<v8::Object> math_table) {
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
                // TODO: inverse
                {FTT_Prototype, "translate", f.NewFunctionTemplate(Translate)},
                {FTT_Prototype, "scale", f.NewFunctionTemplate(Scale)},
                {FTT_Prototype, "rotate", f.NewFunctionTemplate(Rotate)},
                {FTT_Prototype, "toString", f.NewFunctionTemplate(ToString)}
            });
            
            matrix_template->InstanceTemplate()->SetInternalFieldCount(1);
            
            matrix_template->SetClassName(f.NewString("Matrix"));
            
            _matrixInstance.Reset(f.GetIsolate(), matrix_template);
            
            math_table->Set(f.NewString("Matrix"), matrix_template->GetFunction());
        }
        
        void DegToRad(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsNumber(), "Arg0 is the number in degrees to convert to radians")) return;
            
            args.SetReturnValue(args.NewNumber(glm::radians(args.NumberValue(0))));
        }
        
        void RadToDeg(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsNumber(), "Arg0 is the number in radians to convert to degrees")) return;
            
            args.SetReturnValue(args.NewNumber(glm::degrees(args.NumberValue(0))));
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
                {FTT_Static, "degToRad", f.NewFunctionTemplate(DegToRad)->GetFunction()},
                {FTT_Static, "radToDeg", f.NewFunctionTemplate(RadToDeg)->GetFunction()}
                // TODO: noise(glm_noise, perlin, periodicPerlin, simplex)
                // TODO: noiseArray(glm_noise, perlin, periodicPerlin, simplex)
            });
            
            JS_BasicRandom::CreateInterface(f.GetIsolate(), math_table);
            JS_Vector::CreateInterface(f.GetIsolate(), math_table);
            JS_Matrix::CreateInterface(f.GetIsolate(), math_table);
        }
    }
}