/*
   Filename: ScriptingManager.cpp
   Purpose:  Scripting management

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

#include "ScriptingManager.hpp"

#include "Logger.hpp"
#include "Filesystem.hpp"

namespace Engine {
    namespace ScriptingManager {
        
        // It's just the v8 code fitted closer to the engine's coding style
        void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch) {
            std::stringstream ss;
            v8::HandleScope handle_scope(isolate);
            v8::String::Utf8Value exception(try_catch->Exception());
            const char* exception_string = *exception;
            v8::Handle<v8::Message> message = try_catch->Message();
            if (message.IsEmpty()) {
                // V8 didn't provide any extra information about this error; just
                // print the exception.
                ss << exception_string << std::endl;
            } else {
                // Print (filename):(line number): (message).
                v8::String::Utf8Value filename(message->GetScriptResourceName());
                const char* filename_string = *filename;
                int linenum = message->GetLineNumber();
                ss << filename_string << ":" << linenum << ": " << exception_string << std::endl;
                // Print line of source code.
                v8::String::Utf8Value sourceline(message->GetSourceLine());
                const char* sourceline_string = *sourceline;
                ss << sourceline_string << std::endl;
                // Print wavy underline (GetUnderline is deprecated).
                int start = message->GetStartColumn();
                for (int i = 0; i < start; i++) {
                    ss << " ";
                }
                int end = message->GetEndColumn();
                for (int i = start; i < end; i++) {
                    ss << "^";
                }
                v8::String::Utf8Value stack_trace(try_catch->StackTrace());
                if (stack_trace.length() > 0) {
                    const char* stack_trace_string = *stack_trace;
                    ss << std::endl << stack_trace_string;
                }
            }
            Logger::LogText("Scripting", Logger::LogLevel_Error, ss.str());
        }
        
        Json::Value _getValueFromV8Object(v8::Local<v8::Value> val) {
            if (val->IsNull() || val->IsUndefined()) { return Json::Value(Json::nullValue); }
            else if (val->IsNumber()) { return Json::Value(val->NumberValue()); }
            else if (val->IsString()) { return Json::Value(*v8::String::Utf8Value(val)); }
            else if (val->IsBoolean()) { return Json::Value(val->BooleanValue()); }
            else if (val->IsArray()) {
                v8::Local<v8::Object> obj = val.As<v8::Object>();
                v8::Local<v8::Array> objNames = obj->GetPropertyNames();
                
                Json::Value ret(Json::arrayValue);
                
                for (int i = 0; i < objNames->Length(); i++) {
                    v8::Local<v8::Value> objItem = obj->Get(i);
                    ret[i] = _getValueFromV8Object(objItem);
                }
                return ret;
            } else if (val->IsObject()) {
                v8::Local<v8::Object> obj = val.As<v8::Object>();
                v8::Local<v8::Array> objNames = obj->GetPropertyNames();
                
                Json::Value ret(Json::objectValue);
                
                for (int i = 0; i < objNames->Length(); i++) {
                    v8::Local<v8::String> objKey = objNames->Get(i)->ToString();
                    v8::Local<v8::Value> objItem = obj->Get(objKey);
                    ret[*v8::String::Utf8Value(objKey)] = _getValueFromV8Object(objItem);
                }
                return ret;
            } else {
                throw "_getValueFromV8Object type unimplamented";
            }
        }
        
        Json::Value ObjectToJson(v8::Local<v8::Object> obj) {
            return _getValueFromV8Object(obj);
        }
        
        v8::Local<v8::Value> _getValueFromJson(Json::Value val) {
            v8::Isolate* isolate = v8::Isolate::GetCurrent();
            switch (val.type()) {
                case Json::nullValue: return v8::Null(isolate);
                case Json::intValue: return v8::Number::New(isolate, val.asInt());
                case Json::uintValue: return v8::Number::New(isolate, val.asUInt());
                case Json::realValue: return v8::Number::New(isolate, val.asDouble());
                case Json::stringValue: return v8::String::NewFromUtf8(isolate, val.asCString());
                case Json::booleanValue: return v8::Boolean::New(isolate, val.asBool());
                case Json::arrayValue: {
                    v8::Local<v8::Array> ret = v8::Array::New(isolate);
                    for (auto iter = val.begin(); iter != val.end(); iter++) {
                        ret->Set(_getValueFromJson(iter.key()), _getValueFromJson(val[iter.key().asString()]));
                    }
                    return ret;
                }
                case Json::objectValue: {
                    v8::Local<v8::Object> ret = v8::Object::New(isolate);
                    for (auto iter = val.begin(); iter != val.end(); iter++) {
                        ret->Set(_getValueFromJson(iter.key()), _getValueFromJson(val[iter.key().asString()]));
                    }
                    return ret;
                }
                default:
                    throw "_getValueFromJson type unimplamented";
            }
        }
        
        v8::Local<v8::Object> GetObjectFromJson(Json::Value val) {
            return _getValueFromJson(val).As<v8::Object>();
        }
    }
}