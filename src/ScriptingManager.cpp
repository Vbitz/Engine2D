#include "ScriptingManager.hpp"

#include "Logger.hpp"

namespace Engine {
    namespace ScriptingManager {
        // ScriptingContext
        
        ScriptingContext::ScriptingContext() {
            
        }
        
        ScriptingContext::~ScriptingContext() {
            
        }
        
        bool ScriptingContext::Create() {
            if (!_globalInit()) {
                Logger::begin("ScriptingContext", Logger::LogLevel_Error) << "Could not preform global init on ScriptingContext" << Logger::end();
                return false;
            }
            
            // TODO: Create all methods using Context specfic API
            
            // TODO: Run bootloader
        }
        
        void ScriptingContext::RunFile(std::string filename) {
            
        }
        
        void ScriptingContext::Set(std::string str, ScriptingObject* obj) {
            this->_setGlobal(str, obj);
        }
        
        ScriptingObject* ScriptingContext::operator[](std::string name) {
            return this->_getGlobal(name);
        }
        
        
        // ScriptingObject
        
        void ScriptingObject::Set(std::string str, ScriptingObject *obj) {
            this->_setChild(str, obj);
        }
        
        ScriptingObject* FunctionCallbackArgs::operator[](int index) {
            return this->_getArg(index);
        }
        
        // V8ScriptingContext
        
        void V8ScriptingContext::RunString(std::string code, std::string sourceFile) {
            
        }
        
        ScriptingObject* V8ScriptingContext::CreateObject(ObjectType type) {
            return new V8ScriptingObject();
        }
        
        ScriptingObject* V8ScriptingContext::CreateFunction(ScriptingFunctionCallback cb) {
            return new V8ScriptingObject();
        }
        
        bool V8ScriptingContext::_globalInit() {
            return true;
        }
        
        void V8ScriptingContext::_setGlobal(std::string name, ScriptingObject* obj) {
            
        }
        
        ScriptingObject* V8ScriptingContext::_getGlobal(std::string name) {
            return new V8ScriptingObject();
        }
        
        // V8ScriptingObject
        
        V8ScriptingObject::V8ScriptingObject() {
            
        }
        
        double V8ScriptingObject::GetNumberValue(double defaultValue) {
            return defaultValue;
        }
        
        std::string V8ScriptingObject::GetStringValue(std::string defaultValue) {
            return defaultValue;
        }
        
        bool V8ScriptingObject::GetBooleanValue(bool defaultValue) {
            return defaultValue;
        }
        
        bool V8ScriptingObject::Call(ScriptingContext* context, ScriptingObject* args) {
            return true;
        }
        
        int V8ScriptingObject::Length() {
            return 0;
        }
        
        ScriptingObject* V8ScriptingObject::_getChild(std::string name) {
            return new V8ScriptingObject();
        }
        
        void V8ScriptingObject::_setChild(std::string name, ScriptingObject* obj) {
            
        }
        
        // globals
        
        ScriptingContext* CreateScriptingContext(std::string providorName) {
            return new V8ScriptingContext();
        }
        
        Json::Value _getValueFromV8Object(v8::Handle<v8::Value> val) {
            if (val->IsNull() || val->IsUndefined()) { return Json::Value(Json::nullValue); }
            else if (val->IsNumber()) { return Json::Value(val->NumberValue()); }
            else if (val->IsString()) { return Json::Value(*v8::String::Utf8Value(val)); }
            else if (val->IsBoolean()) { return Json::Value(val->BooleanValue()); }
            else if (val->IsArray() || val->IsObject()) {
                throw "_getValueFromV8Object array/object unimplamented";
            } else {
                throw "_getValueFromV8Object type unimplamented";
            }
        }
        
        Json::Value ObjectToJson(v8::Handle<v8::Object> obj) {
            v8::HandleScope scp(v8::Isolate::GetCurrent());
            return _getValueFromV8Object(obj);
        }
        
        v8::Handle<v8::Value> _getValueFromJson(Json::Value val) {
            switch (val.type()) {
                case Json::nullValue: return v8::Null();
                case Json::intValue: return v8::Number::New(val.asInt());
                case Json::uintValue: return v8::Number::New(val.asUInt());
                case Json::realValue: return v8::Number::New(val.asDouble());
                case Json::stringValue: return v8::String::New(val.asCString());
                case Json::booleanValue: return v8::Boolean::New(val.asBool());
                case Json::arrayValue:
                case Json::objectValue:
                    v8::Handle<v8::Array> ret = v8::Array::New();
                    for (auto iter = val.begin(); iter != val.end(); iter++) {
                        ret->Set(_getValueFromJson(iter.key()), _getValueFromJson(val[iter.key().asString()]));
                    }
                    return ret;
            }
        }
        
        v8::Handle<v8::Object> GetObjectFromJson(Json::Value val) {
            v8::HandleScope scp(v8::Isolate::GetCurrent());
            return scp.Close(v8::Handle<v8::Object>::Cast(_getValueFromJson(val)));
        }
    }
}