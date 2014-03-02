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
        
        ScriptingObject* ScriptingContext::operator[](std::string name) {
            return this->_getGlobal(name);
        }
        
        
        // ScriptingObject
        
        // V8ScriptingContext
        
        bool V8ScriptingContext::_globalInit() {
            return true;
        }
        
        void V8ScriptingContext::_runString(std::string code, std::string sourceFile) {
            // TODO: this is basicly just runFile modded from Application
        }
        
        // V8ScriptingObject
        
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