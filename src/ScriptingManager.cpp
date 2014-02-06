#include "ScriptingManager.hpp"

#include <assert.h>
#include <iostream>

namespace Engine {
    namespace ScriptingManager {
        Json::Value ObjectToJson(v8::Handle<v8::Object> obj) {
            v8::HandleScope scp(v8::Isolate::GetCurrent());
            Json::Value ret(Json::objectValue);
            
            return ret;
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