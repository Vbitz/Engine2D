/*
   Filename: ScriptingManager.hpp
   Purpose:  Scripting Management

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

#include <unordered_map>
#include <iostream>

#include "stdlib.hpp"
#include "Scripting.hpp"
#include "vendor/json/json.h"

#define SCRIPTINGMANAGER_INLINE

namespace Engine {
    namespace ScriptingManager {
        void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch);
        
        class Arguments {
        public:
            Arguments(const v8::FunctionCallbackInfo<v8::Value>& args) : _args(args), _scope(args.GetIsolate()), _isolate(args.GetIsolate()) {
            }
            
            SCRIPTINGMANAGER_INLINE v8::Handle<v8::String> NewString(const char* str) {
                return v8::String::NewFromUtf8(this->_isolate, str);
            }
            
            SCRIPTINGMANAGER_INLINE v8::Handle<v8::String> NewString(std::string str) {
                return this->NewString(str.c_str());
            }
            
            SCRIPTINGMANAGER_INLINE v8::Handle<v8::Number> NewNumber(double value) {
                return v8::Number::New(this->_isolate, value);
            }
            
            SCRIPTINGMANAGER_INLINE v8::Handle<v8::Number> NewNumber(int value) {
                return v8::Number::New(this->_isolate, value);
            }
            
            SCRIPTINGMANAGER_INLINE v8::Handle<v8::Array> NewArray() {
                return v8::Array::New(this->_isolate);
            }
            
            SCRIPTINGMANAGER_INLINE v8::Handle<v8::Object> NewObject() {
                return v8::Object::New(this->_isolate);
            }
            
            SCRIPTINGMANAGER_INLINE void ThrowError(const char* msg) {
                this->_isolate->ThrowException(v8::Exception::Error(this->NewString(msg)));
            }
            
            SCRIPTINGMANAGER_INLINE void ThrowError(std::string msg) {
                this->_isolate->ThrowException(v8::Exception::Error(this->NewString(msg)));
            }
            
            SCRIPTINGMANAGER_INLINE void ThrowArgError(const char* msg) {
                this->_isolate->ThrowException(v8::Exception::TypeError(this->NewString(msg)));
            }
            
            SCRIPTINGMANAGER_INLINE void ThrowArgError(std::string msg) {
                this->_isolate->ThrowException(v8::Exception::TypeError(this->NewString(msg)));
            }
            
            SCRIPTINGMANAGER_INLINE bool Assert(bool value, const char* msg) {
                if (!value) {
                    this->ThrowArgError(msg);
                }
                return !value;
            }
            
            SCRIPTINGMANAGER_INLINE bool AssertCount(size_t count) {
                return this->Assert(this->Length() == count, "Wrong number of arguments");
            }
            
            SCRIPTINGMANAGER_INLINE std::string StringValue(int arg) {
                return std::string(*v8::String::Utf8Value(this->_args[arg]));
            }
            
            SCRIPTINGMANAGER_INLINE v8::Isolate* GetIsolate() {
                return this->_args.GetIsolate();
            }
            
            SCRIPTINGMANAGER_INLINE size_t Length() {
                return this->_args.Length();
            }
            
            SCRIPTINGMANAGER_INLINE void SetReturnValue(v8::Handle<v8::Value> ret) {
                this->_args.GetReturnValue().Set(ret);
            }
            
            SCRIPTINGMANAGER_INLINE v8::Handle<v8::Value> operator[](int index) {
                return this->_args[index];
            }
            
        private:
            const v8::FunctionCallbackInfo<v8::Value>& _args;
            v8::Isolate* _isolate;
            v8::HandleScope _scope;
        };
        
        Json::Value ObjectToJson(v8::Local<v8::Object> obj);
        v8::Local<v8::Object> GetObjectFromJson(Json::Value val);
    }
}