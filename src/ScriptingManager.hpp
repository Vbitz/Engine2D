#pragma once

#include "unordered_map"

#include "Scripting.hpp"
#include "vendor/json/json.h"

namespace Engine {
    namespace ScriptingManager {
        class ScriptingObject;
        
        enum ObjectType {
            ObjectType_Null,
            ObjectType_Number,
            ObjectType_String,
            ObjectType_Boolean,
            ObjectType_Function,
            ObjectType_Array,
            ObjectType_Object
        };
        
        class ScriptingContext {
        public:
            ScriptingContext();
            ~ScriptingContext();
            
            bool Create();
            void RunFile(std::string filename, bool persist);
            
            ScriptingObject* operator[](std::string name);
        protected:
            virtual bool _globalInit() = 0;
            virtual void _runString(std::string code, std::string sourceFile) = 0;
            
            virtual ScriptingObject* _getGlobal(std::string name);
            
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
            
            virtual int Length();
            
        protected:
            
        private:
            ScriptingContext* parent;
            ObjectType _type = ObjectType_Null;
        };
        
        class V8ScriptingContext : public ScriptingContext {
        public:
            
        protected:
            bool _globalInit() override;
            void _runString(std::string code, std::string sourceFile) override;
            
        private:
            
        };
        
        class V8ScriptingObject : public ScriptingObject {
        public:
            V8ScriptingObject();
        protected:
            
        private:
            
        };
        
        ScriptingContext* CreateScriptingContext(std::string providorName);
        
        Json::Value ObjectToJson(v8::Handle<v8::Object> obj);
        v8::Handle<v8::Object> GetObjectFromJson(Json::Value val);
    }
}