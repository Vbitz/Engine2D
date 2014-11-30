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

#include <assert.h>

#include <unordered_map>
#include <iostream>

#include "stdlib.hpp"
#include "Scripting.hpp"
#include "vendor/json/json.h"

#define SCRIPTINGMANAGER_INLINE inline

#define ENGINE_SCRIPT_METHOD(name)
#define E_SCRIPT_SIGNATURE(name) name ## _V8
#define _ENGINE_SCRIPT_METHOD_SIGNATURE(name) void E_SCRIPT_SIGNATURE(name)(const v8::FunctionCallbackInfo<v8::Value>& _args)

namespace Engine {
    enum FunctionTemplateTemplates { // It's way too long as an enum class
        FTT_Prototype,
        FTT_Instance,
        FTT_Static
    };
    
    namespace ScriptingManager {
        void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch);
        
        struct FunctionTemplateValues {
            FunctionTemplateTemplates location;
            const char* key;
            v8::Handle<v8::Data> value;
        };
        
        struct ObjectValues {
            FunctionTemplateTemplates location;
            const char* key;
            v8::Handle<v8::Value> value;
        };
        
        ENGINE_CLASS(Factory);
        
        class Factory {
        public:
            Factory(v8::Isolate* isolate) : _isolate(isolate), _scope(isolate) {
                
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
            
            SCRIPTINGMANAGER_INLINE v8::Handle<v8::Number> NewInt32(int value) {
                return v8::Number::New(this->_isolate, value);
            }
            
            SCRIPTINGMANAGER_INLINE v8::Handle<v8::Array> NewArray() {
                return v8::Array::New(this->_isolate);
            }
            
            SCRIPTINGMANAGER_INLINE v8::Handle<v8::Object> NewObject() {
                return v8::Object::New(this->_isolate);
            }
            
            SCRIPTINGMANAGER_INLINE v8::Handle<v8::External> NewExternal(void* value) {
                return v8::External::New(this->_isolate, value);
            }
            
            SCRIPTINGMANAGER_INLINE v8::Handle<v8::Boolean> NewBoolean(bool value) {
                return v8::Boolean::New(this->_isolate, value);
            }
            
            SCRIPTINGMANAGER_INLINE v8::Handle<v8::FunctionTemplate> NewFunctionTemplate(v8::FunctionCallback callback) {
                return v8::FunctionTemplate::New(this->_isolate, callback);
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
                assert(msg != "");
                if (!value) {
                    this->ThrowArgError(msg);
                }
                return !value;
            }
            
            SCRIPTINGMANAGER_INLINE v8::Isolate* GetIsolate() {
                return this->_isolate;
            }

            SCRIPTINGMANAGER_INLINE void FillObject(v8::Handle<v8::Object> handle, std::vector<ObjectValues> values) {
                for (auto iter = values.begin(); iter != values.end(); iter++) {
                    switch (iter->location) {
                        case FTT_Static:
                            handle->Set(this->NewString(iter->key), iter->value);
                            break;
                        default:
                            break;
                    }
                }
            }
            
            SCRIPTINGMANAGER_INLINE void FillTemplate(v8::Handle<v8::ObjectTemplate> handle, std::vector<FunctionTemplateValues> values) {
                for (auto iter = values.begin(); iter != values.end(); iter++) {
                    switch (iter->location) {
                        case FTT_Static:
                            handle->Set(this->NewString(iter->key), iter->value);
                            break;
                        default:
                            break;
                    }
                }
            }
            
            SCRIPTINGMANAGER_INLINE void FillTemplate(v8::Handle<v8::FunctionTemplate> handle, std::vector<FunctionTemplateValues> values) {
                for (auto iter = values.begin(); iter != values.end(); iter++) {
                    switch (iter->location) {
                        case FTT_Prototype:
                            handle->PrototypeTemplate()->Set(this->NewString(iter->key), iter->value);
                            break;
                        case FTT_Instance:
                            handle->InstanceTemplate()->Set(this->NewString(iter->key), iter->value);
                            break;
                        case FTT_Static:
                            handle->Set(this->NewString(iter->key), iter->value);
                            break;
                    }
                }
            }
        private:
            v8::Isolate* _isolate;
            v8::HandleScope _scope;
        };
        
        ENGINE_CLASS(ObjectWrap);
        
        // Very closely related to the Node.js ObjectWrap with an easier API
        // https://github.com/joyent/node/blob/master/src/node_object_wrap.h
        class ObjectWrap {
        public:
            virtual ~ObjectWrap() {
                if (this->_handle.IsEmpty()) {
                    return;
                }
                assert(this->_handle.IsNearDeath());
                this->_handle.ClearWeak();
                this->_handle.Reset();
            }
            
            template<class T>
            static inline auto Wrap(v8::Isolate* isolate, v8::Handle<v8::Object> handle) -> T* {
                T* instance = new T();
                ObjectWrapPtr wrap = static_cast<ObjectWrapPtr>(instance);
                assert(wrap->_handle.IsEmpty());
                assert(handle->InternalFieldCount() > 0);
                handle->SetAlignedPointerInInternalField(0, instance);
                wrap->_handle.Reset(isolate, handle);
                wrap->_handle.SetWeak(wrap, WeakCallback);
                wrap->_handle.MarkIndependent();
                return instance;
            }
            
            template<class T>
            static inline auto Unwrap(v8::Handle<v8::Object> handle) -> T* {
                assert(!handle.IsEmpty());
                void* ptr = handle->GetAlignedPointerFromInternalField(0);
                ObjectWrapPtr wrap = static_cast<ObjectWrapPtr>(ptr);
                return static_cast<T*>(wrap);
            }
        private:
            
            static void WeakCallback(const v8::WeakCallbackData<v8::Object, ObjectWrap>& data) {
                v8::Isolate* isolate = data.GetIsolate();
                v8::HandleScope scope(isolate);
                ObjectWrapPtr wrap = data.GetParameter();
                assert(wrap->_handle.IsNearDeath());
                assert(data.GetValue() == v8::Local<v8::Object>::New(isolate, wrap->_handle));
                wrap->_handle.Reset();
                delete wrap;
            }
            
            v8::Persistent<v8::Object> _handle;
        };
        
        ENGINE_CLASS(Arguments);
        
        class Arguments : public Factory {
        public:
            Arguments(const v8::FunctionCallbackInfo<v8::Value>& args) : _args(args), Factory(args.GetIsolate()) {
            }
            
            SCRIPTINGMANAGER_INLINE bool AssertCount(size_t count) {
                return this->Assert(this->Length() == count, "Wrong number of arguments");
            }
            
            SCRIPTINGMANAGER_INLINE const char* CStringValue(int arg) {
                return *v8::String::Utf8Value(this->_args[arg]);
            }
            
            SCRIPTINGMANAGER_INLINE std::string StringValue(int arg) {
                return std::string(*v8::String::Utf8Value(this->_args[arg]));
            }
            
            SCRIPTINGMANAGER_INLINE uint32_t Int32Value(int arg) {
                return this->_args[arg]->Int32Value();
            }
            
            SCRIPTINGMANAGER_INLINE double NumberValue(int arg) {
                return this->_args[arg]->NumberValue();
            }
            
            SCRIPTINGMANAGER_INLINE bool BooleanValue(int arg) {
                return this->_args[arg]->BooleanValue();
            }
            
            SCRIPTINGMANAGER_INLINE void* ExternalValue(int arg) {
                return v8::Handle<v8::External>::Cast(this->_args[arg])->Value();
            }
            
            SCRIPTINGMANAGER_INLINE bool RecallAsConstructor() {
                if (!this->IsConstructCall()) {
                    const int argc = this->_args.Length();
                    std::vector<v8::Handle<v8::Value>> av(static_cast<size_t>(argc),v8::Undefined(this->GetIsolate()));
                    for(int i = 0; i < argc; ++i) av[i] = this->_args[i];
                    this->SetReturnValue(this->_args.Callee()->NewInstance(argc, &av[0]));
                    return true;
                } else {
                    return false;
                }
            }
            
            SCRIPTINGMANAGER_INLINE bool IsConstructCall() {
                return this->_args.IsConstructCall();
            }
            
            SCRIPTINGMANAGER_INLINE size_t Length() {
                return this->_args.Length();
            }
            
            SCRIPTINGMANAGER_INLINE void SetReturnValue(v8::Handle<v8::Value> ret) {
                this->_args.GetReturnValue().Set(ret);
            }
            
            SCRIPTINGMANAGER_INLINE v8::Local<v8::Object> This() {
                return this->_args.This();
            }
            
            SCRIPTINGMANAGER_INLINE v8::Local<v8::Function> GetConstructor() {
                v8::Handle<v8::Value> ctor = This()->GetPrototype()->ToObject()->Get(this->NewString("constructor"));
                return v8::Handle<v8::Function>::Cast(ctor);
            }
            
            SCRIPTINGMANAGER_INLINE v8::Handle<v8::Value> operator[](int index) {
                return this->_args[index];
            }
            
        private:
            const v8::FunctionCallbackInfo<v8::Value>& _args;
        };
        
        ENGINE_CLASS(Context);
        
        class Context {
        public:
            Context();
            ~Context();
            
            bool RunFile(std::string path, bool persist);
            void RunCommand(std::string str);
            void InvalidateScript(std::string filename);
            void CheckUpdate();
            v8::Local<v8::Object> GetScriptTable(std::string name);
            
            v8::Isolate* GetIsolate() {
                return this->_isolate;
            }
            
            static void StaticInit();
            static void SetFlag(std::string flag);
            static void RunHelpCommand();
            static void TriggerGC();
            void TraceGlobalEnviroment();
        private:
            v8::Isolate* _isolate = NULL;
            v8::HandleScope _scope;
            
            void _enableTypedArrays();
            void _enableHarmony();
            void _createEventMagic();
            v8::Handle<v8::Context> _initScripting();
            void _disablePreload();
            
            bool _runFile(std::string filename, bool persist);
            
            static void _v8PostGCCallback(v8::Isolate* isolate, v8::GCType type, v8::GCCallbackFlags flags);
            
            std::map<std::string, long> _loadedFiles;
        };
        
        Json::Value ObjectToJson(v8::Local<v8::Object> obj);
        v8::Local<v8::Object> GetObjectFromJson(Json::Value val);
    }
}