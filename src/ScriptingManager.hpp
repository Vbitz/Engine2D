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

#include "stdlib.hpp"
#include "Scripting.hpp"
#include "vendor/json/json.h"

namespace Engine {
    namespace ScriptingManager {
        ENGINE_CLASS(ScriptingObject);
        ENGINE_CLASS(FunctionCallbackArgs);
        
        enum ObjectType {
            ObjectType_Invalid,
            ObjectType_Null,
            ObjectType_Number,
            ObjectType_String,
            ObjectType_Boolean,
            ObjectType_Function,
            ObjectType_Array,
            ObjectType_Object
        };
        
        typedef void (*ScriptingFunctionCallback)(FunctionCallbackArgsRef);
        
        void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch);
        
        ENGINE_CLASS(ScriptingContext);
        
        class ScriptingContext {
        public:
            ScriptingContext();
            ~ScriptingContext();
            
            bool Create();
            bool RunFile(std::string filename);
            
            virtual bool RunString(std::string code, std::string sourceFile) = 0;
            
            void Set(std::string str, ScriptingObjectPtr obj);
            
            virtual ScriptingObjectPtr CreateObject(ObjectType type) = 0;
            virtual ScriptingObjectPtr CreateFunction(ScriptingFunctionCallback cb) = 0;
            
            virtual void Enter() = 0;
            virtual void Exit() = 0;
            
            ScriptingObjectPtr operator[](std::string name);
        protected:
            virtual bool _globalInit() = 0;
            
            virtual void _setGlobal(std::string name, ScriptingObjectPtr obj) = 0;
            virtual ScriptingObjectPtr _getGlobal(std::string name) = 0;
            
        private:
            bool _created = false;
            bool _destroyed = false;
        };
        
        ENGINE_CLASS(ScriptingObject);
        
        class ScriptingObject {
        public:
            
            ObjectType GetType();
            
            bool IsNull() { return this->_type = ObjectType_Null; }
            bool IsNumber() { return this->_type = ObjectType_Number; }
            bool IsString() { return this->_type = ObjectType_String; }
            bool IsBoolean() { return this->_type = ObjectType_Boolean; }
            bool IsFunction() { return this->_type = ObjectType_Function; }
            bool IsArray() { return this->_type = ObjectType_Array; }
            bool IsObject() { return this->_type = ObjectType_Object; }
            
            virtual double GetNumberValue(double defaultValue = 0.0) = 0;
            virtual std::string GetStringValue(std::string defaultValue = "") = 0;
            virtual bool GetBooleanValue(bool defaultValue = false) = 0;
            
            virtual bool Call(ScriptingContextPtr context, ScriptingObjectPtr args) = 0;
            
            virtual int Length() = 0;
            
            void Set(std::string str, ScriptingObjectPtr obj);
            
            ScriptingObjectPtr operator[](std::string name);
            
        protected:
            
            virtual ScriptingObject* _getChild(std::string name) = 0;
            virtual void _setChild(std::string name, ScriptingObjectPtr obj) = 0;
            void _setType(ObjectType type);
            
        private:
            ScriptingContextPtr parent;
            ObjectType _type = ObjectType_Invalid;
        };
        
        class FunctionCallbackArgs {
        public:
            ScriptingObjectPtr operator[](int index);
            
            virtual int Length();
        protected:
            
            virtual ScriptingObjectPtr _getArg(int index);
        private:
            
        };
        
        class V8ScriptingContext : public ScriptingContext {
        public:
            bool RunString(std::string code, std::string sourceFile) override;
            
            ScriptingObjectPtr CreateObject(ObjectType type) override;
            ScriptingObjectPtr CreateFunction(ScriptingFunctionCallback cb) override;
            
            void Enter() override;
            void Exit() override;
        protected:
            bool _globalInit() override;
            
            void _setGlobal(std::string name, ScriptingObjectPtr obj) override;
            ScriptingObjectPtr _getGlobal(std::string name) override;
        private:
            v8::Isolate* _isolate = NULL;
        };
        
        class V8ScriptingObject : public ScriptingObject {
        public:
            V8ScriptingObject(v8::Handle<v8::Value> value);
            
            double GetNumberValue(double defaultValue = 0.0) override;
            std::string GetStringValue(std::string defaultValue = "") override;
            bool GetBooleanValue(bool defaultValue = false) override;
            
            bool Call(ScriptingContextPtr context, ScriptingObjectPtr args) override;
            
            int Length() override;
        protected:
            ScriptingObjectPtr _getChild(std::string name) override;
            void _setChild(std::string name, ScriptingObjectPtr obj) override;
        private:
            
        };
        
        class V8FunctionCallbackArgs : public FunctionCallbackArgs {
        public:
            V8FunctionCallbackArgs(const v8::FunctionCallbackInfo<v8::Value>& args) {
                
            }
        };
        
        ScriptingContextPtr CreateScriptingContext(std::string providorName);
        
        Json::Value ObjectToJson(v8::Local<v8::Object> obj);
        v8::Local<v8::Object> GetObjectFromJson(Json::Value val);
    }
}