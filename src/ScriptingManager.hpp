#pragma once

#include "unordered_map"

#include "Scripting.hpp"
#include "vendor/json/json.h"

namespace Engine {
    namespace ScriptingManager {
        class ScriptingObject;
        class FunctionCallbackArgs;
        
        enum ObjectType {
            ObjectType_Null,
            ObjectType_Number,
            ObjectType_String,
            ObjectType_Boolean,
            ObjectType_Function,
            ObjectType_Array,
            ObjectType_Object
        };
        
        typedef void (*ScriptingFunctionCallback)(FunctionCallbackArgs&);
        
        class ScriptingContext {
        public:
            ScriptingContext();
            ~ScriptingContext();
            
            bool Create();
            void RunFile(std::string filename);
            
            virtual void RunString(std::string code, std::string sourceFile) = 0;
            
            void Set(std::string str, ScriptingObject* obj);
            
            virtual ScriptingObject* CreateObject(ObjectType type) = 0;
            virtual ScriptingObject* CreateFunction(ScriptingFunctionCallback cb) = 0;
            
            ScriptingObject* operator[](std::string name);
        protected:
            virtual bool _globalInit() = 0;
            
            virtual void _setGlobal(std::string name, ScriptingObject* obj) = 0;
            virtual ScriptingObject* _getGlobal(std::string name) = 0;
            
        private:
            bool _created = false;
            bool _destroyed = false;
        };
        
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
            
            virtual bool Call(ScriptingContext* context, ScriptingObject* args) = 0;
            
            virtual int Length() = 0;
            
            void Set(std::string str, ScriptingObject* obj);
            
            ScriptingObject* operator[](std::string name);
            
        protected:
            
            virtual ScriptingObject* _getChild(std::string name) = 0;
            virtual void _setChild(std::string name, ScriptingObject* obj) = 0;
            
        private:
            ScriptingContext* parent;
            ObjectType _type = ObjectType_Null;
        };
        
        class FunctionCallbackArgs {
        public:
            ScriptingObject* operator[](int index);
            
            virtual int Length();
        protected:
            
            virtual ScriptingObject* _getArg(int index);
        private:
            
        };
        
        class V8ScriptingContext : public ScriptingContext {
        public:
            void RunString(std::string code, std::string sourceFile) override;
            
            ScriptingObject* CreateObject(ObjectType type) override;
            ScriptingObject* CreateFunction(ScriptingFunctionCallback cb) override;
        protected:
            bool _globalInit() override;
            
            void _setGlobal(std::string name, ScriptingObject* obj) override;
            ScriptingObject* _getGlobal(std::string name) override;
        private:
            
        };
        
        class V8ScriptingObject : public ScriptingObject {
        public:
            V8ScriptingObject();
            
            double GetNumberValue(double defaultValue = 0.0) override;
            std::string GetStringValue(std::string defaultValue = "") override;
            bool GetBooleanValue(bool defaultValue = false) override;
            
            bool Call(ScriptingContext* context, ScriptingObject* args) override;
            
            int Length() override;
        protected:
            ScriptingObject* _getChild(std::string name) override;
            void _setChild(std::string name, ScriptingObject* obj) override;
        private:
            
        };
        
        class V8FunctionCallbackArgs : public FunctionCallbackArgs {
        public:
            V8FunctionCallbackArgs(const v8::FunctionCallbackInfo<v8::Value>& args) {
                
            }
        };
        
        ScriptingContext* CreateScriptingContext(std::string providorName);
        
        Json::Value ObjectToJson(v8::Handle<v8::Object> obj);
        v8::Handle<v8::Object> GetObjectFromJson(Json::Value val);
    }
}