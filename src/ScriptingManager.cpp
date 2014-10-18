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

#include <cstring>
#include <unordered_map>

#include <include/libplatform/libplatform.h>

// Scripting Includes
#include "JSSys.hpp"
#include "JSDraw.hpp"
#include "JSInput.hpp"
#include "JSFS.hpp"
#include "JSDatabase.hpp"
#include "JSMathHelper.hpp"

#include "Events.hpp"
#include "Logger.hpp"
#include "Config.hpp"
#include "Filesystem.hpp"
#include "Application.hpp"
#include "Profiler.hpp"

class MallocArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
public:
    virtual void* Allocate(size_t length) { return malloc(length); }
    virtual void* AllocateUninitialized(size_t length) { return malloc(length); }
    virtual void Free(void* data) { free(data); }
    virtual void Free(void* data, size_t length) { free(data); }
};

namespace Engine {
    namespace ScriptingManager {
        class EngineV8Platform : public v8::Platform {
        public:
            static void *BackgroundThread(void *threadArg) {
                EngineV8Platform *self = (EngineV8Platform*) threadArg;
                v8::Task *task = NULL;
                while (true) {
                    self->_backgroundMutex->Enter();
                    if (self->_backgroundTasks.size() > 0) {
                        task = self->_backgroundTasks.top();
                        self->_backgroundTasks.pop();
                    }
                    self->_backgroundMutex->Exit();
                    
                    if (task != NULL) {
                        task->Run();
                    }
                    
                    task = NULL;
                    
                    delete task;
                    
                    Engine::Platform::NanoSleep(1000);
                }
            }
            
            EngineV8Platform() {
                _backgroundMutex = Engine::Platform::CreateMutex();
                _backgroundThread = Engine::Platform::CreateThread(BackgroundThread, this);
            }
            
            void CallOnBackgroundThread(v8::Task* task,
                                        ExpectedRuntime expected_runtime) override {
                _backgroundMutex->Enter();
                _backgroundTasks.push(task);
                _backgroundMutex->Exit();
            }
            
            void CallOnForegroundThread(v8::Isolate* isolate, v8::Task* task) override {
                _foregroundTasks[isolate].tasks.push(task);
            }
            
            double MonotonicallyIncreasingTime() override {
                return Engine::Platform::GetTime();
            }
            
            bool PumpMessages(v8::Isolate* isolate) {
                if (_foregroundTasks.count(isolate) == 0) {
                    return false;
                }
                
                TaskList &list = _foregroundTasks[isolate];
                
                if (list.tasks.size() > 0) {
                    v8::Task *task = list.tasks.top();
                    list.tasks.pop();
                    task->Run();
                    delete task;
                    return true;
                } else {
                    return false;
                }
            }
            
        private:
            struct TaskList {
                std::stack<v8::Task*> tasks;
            };
            
            std::unordered_map<v8::Isolate*, TaskList> _foregroundTasks;
            std::stack<v8::Task*> _backgroundTasks;
            Engine::Platform::ThreadPtr _backgroundThread;
            Engine::Platform::MutexPtr _backgroundMutex;
        };
        
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
        
        // ScriptingContext
        
        static v8::Persistent<v8::External> _EM_CANCEL;
        
        static void globalAccessor(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
            info.GetReturnValue().Set(info.Holder());
        }
        
        ENGINE_JS_METHOD(EventCallCallback) {
            ENGINE_JS_SCOPE_OPEN;
            
            Json::Value eventArgs;
            
            if (args.Length() == 0) {
                eventArgs = Json::Value(Json::objectValue);
            } else {
                eventArgs = ScriptingManager::ObjectToJson(args[0].As<v8::Object>());
            }
            
            v8::Handle<v8::String> eventName = args.Data().As<v8::String>();
            
            std::string eventNameStr = std::string(*v8::String::Utf8Value(eventName));
            
            GetEventsSingilton()->GetEvent(eventNameStr)->Emit(eventArgs);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(EventToStringStub) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_JS_SCOPE_CLOSE(v8::String::NewFromUtf8(args.GetIsolate(), "[object Event]"));
        }
        
        static void EventGetterCallback(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value>& info) {
            v8::HandleScope scp(info.GetIsolate());
            
            std::string nameStr = std::string(*v8::String::Utf8Value(name));
            
            if (nameStr == "EM_CANCEL") {
                info.GetReturnValue().Set(_EM_CANCEL);
            } else if (nameStr == "toString") {
                v8::Handle<v8::FunctionTemplate> toStringStub = v8::FunctionTemplate::New(info.GetIsolate(), EventToStringStub);
                info.GetReturnValue().Set(toStringStub->GetFunction());
            } else {
                v8::Handle<v8::FunctionTemplate> t = v8::FunctionTemplate::New(info.GetIsolate());
                
                t->SetCallHandler(EventCallCallback, name);
                
                v8::Handle<v8::Function> func = t->GetFunction();
                
                info.GetReturnValue().Set(func);
            }
        }
        
        static void EventSetterCallback(v8::Local<v8::String> name, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<v8::Value>& info) {
            v8::HandleScope scp(info.GetIsolate());
            
            if (!val->IsFunction()) {
                ENGINE_THROW_ARGERROR("The value passed needs to be a function");
                return;
            }
            
            v8::Handle<v8::Function> func = val.As<v8::Function>();
            
            std::string hookName = std::string(*v8::String::Utf8Value(name));
            std::string hookId = std::string(*v8::String::Utf8Value(func->GetName()));
            
            if (hookId.length() < 1) {
                ENGINE_THROW_ARGERROR("The function passed needs to be named");
                return;
            }
            
            GetEventsSingilton()->GetEvent(hookName)->AddListener(hookId, EventEmitter::MakeTarget(func));
        }
        
        ENGINE_JS_METHOD(ConfigToStringStub) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_JS_SCOPE_CLOSE(v8::String::NewFromUtf8(args.GetIsolate(), "[object Config]"));
        }
        
        static void ConfigGetterCallback(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value>& info) {
            v8::HandleScope scp(info.GetIsolate());
            
            std::string nameStr = std::string(*v8::String::Utf8Value(name));
            
            if (nameStr == "toString") {
                v8::Handle<v8::FunctionTemplate> toStringStub = v8::FunctionTemplate::New(info.GetIsolate(), ConfigToStringStub);
                info.GetReturnValue().Set(toStringStub->GetFunction());
                return;
            }
            
            Config::ConfigType type = Config::GetType(nameStr);
            switch (type) {
                case Config::ConfigType_Bool:
                    info.GetReturnValue().Set(v8::Boolean::New(info.GetIsolate(), Config::GetBoolean(nameStr)));
                    break;
                case Config::ConfigType_Number:
                    info.GetReturnValue().Set(v8::Number::New(info.GetIsolate(), Config::GetFloat(nameStr)));
                    break;
                case Config::ConfigType_String:
                    info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), Config::GetString(nameStr).c_str()));
                    break;
                case Config::ConfigType_Unknown:
                    info.GetReturnValue().Set(v8::Null(info.GetIsolate()));
                    break;
            }
        }
        
        static void ConfigSetterCallback(v8::Local<v8::String> name, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<v8::Value>& info) {
            v8::HandleScope scp(info.GetIsolate());
            
            std::string nameStr = std::string(*v8::String::Utf8Value(name));
            
            Config::ConfigType type = Config::GetType(nameStr);
            
            if (val->IsString() || type == Config::ConfigType_String) {
                Config::SetString(nameStr, std::string(*v8::String::Utf8Value(val)));
            } else if (val->IsNumber() || type == Config::ConfigType_Number) {
                Config::SetNumber(nameStr, (float) val->ToNumber()->Value());
            } else if (val->IsBoolean() || type == Config::ConfigType_Bool) {
                Config::SetBoolean(nameStr, val->ToBoolean()->Value());
            }
        }
        
        EngineV8Platform* platform = NULL;
        
        void Context::StaticInit() {
            if (platform != NULL) {
                return;
            }
            platform = new EngineV8Platform();
            v8::V8::InitializePlatform(platform);
            v8::V8::Initialize();
        }
        
        inline std::string GetGCTypeString(v8::GCType type) {
            switch (type) {
                case v8::kGCTypeAll:
                    return "all";
                case v8::kGCTypeMarkSweepCompact:
                    return "markSweepCompact";
                case v8::kGCTypeScavenge:
                    return "compact";
                default:
                    return "unknown";
            }
        }
        
        void Context::_v8PostGCCallback(v8::Isolate* isolate, v8::GCType type, v8::GCCallbackFlags flags) {
            static Engine::Platform::MutexPtr gcMutex = NULL;
            if (gcMutex == NULL) {
                gcMutex = Engine::Platform::CreateMutex();
            }
            
            gcMutex->Enter();
            
            Json::Value args = Json::Value(Json::objectValue);
            args["type"] = GetGCTypeString(type);
            GetEventsSingilton()->GetEvent("v8_postGC")->Emit(args);
            
            gcMutex->Exit();
        }
        
        Context::Context() : _isolate(v8::Isolate::New()), _scope(this->_isolate) {
            this->_isolate->Enter();
            
            GetEventsSingilton()->GetEvent("v8_postGC")->SetNoScript(true);
            
            this->_isolate->AddGCEpilogueCallback(_v8PostGCCallback);
            
            v8::Handle<v8::Context> ctx = this->_initScripting();
            
            ctx->Enter();
        }
        
        Context::~Context() {
            
        }
        
        void Context::_enableTypedArrays() {
            v8::V8::SetArrayBufferAllocator(new MallocArrayBufferAllocator());
        }
        
        void Context::_enableHarmony() {
            const char* harmony = "-harmony";
            v8::V8::SetFlagsFromString(harmony, std::strlen(harmony));
        }
        
        void Context::_createEventMagic() {
            v8::Isolate* isolate = v8::Isolate::GetCurrent();
            if (_EM_CANCEL.IsEmpty()) {
                _EM_CANCEL.Reset(isolate, v8::External::New(isolate, new EventMagic(EM_CANCEL)));
            }
        }
        
        v8::Handle<v8::Context> Context::_initScripting() {
            Logger::begin("ScriptingContext", Logger::LogLevel_Log) << "Loading Scripting" << Logger::end();
            
            this->_enableTypedArrays();
            this->_enableHarmony();
            this->_createEventMagic();
            
            v8::Isolate* isolate = v8::Isolate::GetCurrent();
            
            v8::EscapableHandleScope handle_scope(isolate);
            
            ScriptingManager::Factory f(isolate);
            
            v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
            
            global->SetAccessor(f.NewString("global"), globalAccessor);
            
            f.FillTemplate(global, {
                {FTT_Static, "assert", f.NewFunctionTemplate(JsSys::Assert)}
            });
            
            // consoleTable
            v8::Handle<v8::ObjectTemplate> consoleTable = v8::ObjectTemplate::New();
            
            f.FillTemplate(consoleTable, {
                {FTT_Static, "_log", f.NewFunctionTemplate(JsSys::Println)},
                {FTT_Static, "clear", f.NewFunctionTemplate(JsSys::ClearConsole)},
                {FTT_Static, "toggle", f.NewFunctionTemplate(JsSys::ToggleConsole)}
            });
            
            global->Set(isolate, "console", consoleTable);
            
            // configTable
            v8::Handle<v8::ObjectTemplate> configTable = v8::ObjectTemplate::New();
            
            configTable->SetNamedPropertyHandler(ConfigGetterCallback, ConfigSetterCallback);
            
            // sysTable
            v8::Handle<v8::ObjectTemplate> sysTable = v8::ObjectTemplate::New();
            
            JsSys::InitSys(sysTable);
            
            sysTable->Set(isolate, "platform", f.NewString(_PLATFORM));
            sysTable->Set(isolate, "devMode", f.NewBoolean(GetAppSingilton()->IsDeveloperMode()));
            sysTable->Set(isolate, "debugMode", f.NewBoolean(GetAppSingilton()->IsDebugMode()));
            sysTable->Set(isolate, "preload", f.NewBoolean(true));
            sysTable->Set(isolate, "numProcessers", f.NewNumber(Platform::GetProcesserCount()));
            
            // depending on the runtime being used in the future this will be set to something unique per system
            // for example on steam it can be the friends name or SteamID
            sysTable->Set(isolate, "username", f.NewString(Platform::GetUsername()));
            
            sysTable->Set(isolate, "runtimeConfig", configTable);
            
            global->Set(isolate, "sys", sysTable);
            
            // eventTable
            v8::Handle<v8::ObjectTemplate> eventTable = v8::ObjectTemplate::New();
            
            eventTable->SetNamedPropertyHandler(EventGetterCallback, EventSetterCallback);
            
            global->Set(isolate, "event", eventTable);
            
            // drawTable
            v8::Handle<v8::ObjectTemplate> drawTable = v8::ObjectTemplate::New();
            
            JsDraw::InitDraw(drawTable);
            
            global->Set(isolate, "draw", drawTable);
            
            // fsTable
            v8::Handle<v8::ObjectTemplate> fsTable = v8::ObjectTemplate::New();
            
            JsFS::InitFS(fsTable);
            
            global->Set(isolate, "fs", fsTable);
            
            // dbTable
            v8::Handle<v8::ObjectTemplate> dbTable = v8::ObjectTemplate::New();
            
            JSDatabase::InitDatabase(dbTable);
            
            global->Set(isolate, "db", dbTable);
            
            // inputTable
            v8::Handle<v8::ObjectTemplate> inputTable = v8::ObjectTemplate::New();
            
            JsInput::InitInput(inputTable);
            
            global->Set(isolate, "input", inputTable);
            
            v8::Local<v8::Context> ctx = v8::Context::New(isolate, NULL, global);
            
            ctx->Enter();
            
            JsMathHelper::InitMathHelper();
            
            if (!this->_runFile(Config::GetString("core.script.loader"), true)) {
                Logger::begin("Scripting", Logger::LogLevel_Error) << "Bootloader not found" << Logger::end();
                GetAppSingilton()->GetEngineUI()->ToggleConsole(); // give them something to debug using
            }
            
            Logger::begin("ScriptingContext", Logger::LogLevel_Log) << "Loaded Scripting" << Logger::end();
            
            return handle_scope.Escape(ctx);
        }
        
        v8::Local<v8::Object> Context::GetScriptTable(std::string name) {
            v8::Local<v8::Context> ctx = this->_isolate->GetCurrentContext();
            
            v8::Local<v8::Object> obj = ctx->Global();
            return v8::Local<v8::Object>::Cast(obj->Get(v8::String::NewFromUtf8(this->_isolate, name.c_str())));
        }
        
        void Context::CheckUpdate() {
            ENGINE_PROFILER_SCOPE;
            if (Config::GetBoolean("core.script.autoReload")) {
                for (auto iterator = this->_loadedFiles.begin(); iterator != this->_loadedFiles.end(); iterator++) {
                    long lastMod = Filesystem::GetFileModifyTime(iterator->first);
                    if (lastMod > iterator->second) {
                        Json::Value args = Json::Value(Json::objectValue);
                        args["filename"] = iterator->first;
                        this->_runFile(iterator->first, true);
                        GetEventsSingilton()->GetEvent("scriptReloaded")->Emit(args);
                    }
                }
            } else {
                for (auto iterator = _loadedFiles.begin(); iterator != _loadedFiles.end(); iterator++) {
                    if (iterator->second < 0) {
                        Json::Value args = Json::Value(Json::objectValue);
                        args["filename"] = iterator->first;
                        this->_runFile(iterator->first, true);
                        GetEventsSingilton()->GetEvent("scriptReloaded")->Emit(args);
                    }
                }
            }
        }
        
        bool Context::_runFile(std::string path, bool persist) {
            ENGINE_PROFILER_SCOPE_EX(path.c_str());
            
            Logger::begin("Scripting", Logger::LogLevel_Verbose) << "Loading File: " << path << Logger::end();
            v8::Isolate* isolate = v8::Isolate::GetCurrent();
            v8::HandleScope scp(isolate);
            v8::Local<v8::Context> ctx = v8::Isolate::GetCurrent()->GetCurrentContext();
            v8::Context::Scope ctx_scope(ctx);
            
            char* inputScript = Filesystem::GetFileContent(path);
            
            v8::TryCatch tryCatch;
            
            v8::Handle<v8::Script> script = v8::Script::Compile(v8::String::NewFromUtf8(isolate, inputScript), v8::String::NewFromUtf8(isolate, path.c_str()));
            
            if (script.IsEmpty()) {
                Logger::begin("Scripting", Logger::LogLevel_Error) << "Could not Load file: " << path << Logger::end();
                ScriptingManager::ReportException(v8::Isolate::GetCurrent(), &tryCatch);
                std::free(inputScript);
                return false;
            } else {
                script->Run();
                if (!tryCatch.StackTrace().IsEmpty()) {
                    ScriptingManager::ReportException(v8::Isolate::GetCurrent(), &tryCatch);
                    std::free(inputScript);
                    return false;
                }
                Logger::begin("Scripting", Logger::LogLevel_Verbose) << "Loaded File: " << path << Logger::end();
                if (persist) {
                    _loadedFiles[path] = Filesystem::GetFileModifyTime(path);
                }
                std::free(inputScript);
                return true;
            }
        }
        
        bool Context::RunFile(std::string path, bool persist) {
            Json::Value eArgs(Json::objectValue);
            eArgs["path"] = path;
            if (GetEventsSingilton()->GetEvent("runFile")->Emit(eArgs) == EM_CANCEL) {
                return true;
            }
            std::string realFile = path + ".js";
            if (!Filesystem::FileExists(realFile)) {
                Logger::begin("Scripting", Logger::LogLevel_Error) << realFile << " Not Found" << Logger::end();
                return false;
            } else {
                if (!_runFile(realFile, persist)) {
                    Logger::begin("Scripting", Logger::LogLevel_Error) << "Could not load File : " << realFile << Logger::end();
                    return false;
                } else {
                    return true;
                }
            }
        }
        
        void Context::RunCommand(std::string str) {
            v8::HandleScope scp(this->_isolate);
            v8::Local<v8::Context> ctx = this->_isolate->GetCurrentContext();
            v8::Context::Scope ctx_scope(ctx);
            
            Logger::begin("Console", Logger::LogLevel_ConsoleInput) << "> " << str << Logger::end();
            
            v8::TryCatch tryCatch;
            
            v8::Handle<v8::Script> script = v8::Script::Compile(v8::String::NewFromUtf8(this->_isolate, str.c_str()), v8::String::NewFromUtf8(this->_isolate, "Console"));
            
            if (script.IsEmpty()) {
                v8::Handle<v8::Value> exception = tryCatch.StackTrace();
                Logger::begin("Console", Logger::LogLevel_Error) << "Exception: " << *v8::String::Utf8Value(exception->ToString()) << Logger::end();
            } else {
                v8::Local<v8::Value> result = script->Run();
                if (*result != NULL && !result->IsExternal()) { // well it works
                    if (result->IsUndefined() && !Config::GetBoolean("core.log.src.undefinedValue")) {
                        
                    } else {
                        Logger::begin("Console", Logger::LogLevel_Log) << (result->IsNull() ? "null" : *v8::String::Utf8Value(result->ToString())) << Logger::end();
                    }
                }
                if (!tryCatch.StackTrace().IsEmpty()) {
                    // Use the old version, the new one is way too long
                    v8::Handle<v8::Value> exception = tryCatch.StackTrace();
                    Logger::begin("Console", Logger::LogLevel_Error) << "Exception: " << *v8::String::Utf8Value(exception->ToString()) << Logger::end();
                }
            }
        }
        
        void Context::InvalidateScript(std::string scriptName) {
            Logger::begin("Scripting", Logger::LogLevel_Verbose) << "Invalidating Script: " << scriptName << Logger::end();
            this->_loadedFiles[scriptName] = -1;
        }
        
        void Context::SetFlag(std::string flag) {
            v8::V8::SetFlagsFromString(flag.c_str(), flag.length());
        }
        
        void Context::RunHelpCommand() {
            v8::V8::SetFlagsFromString("--help", 6);
        }
        
        void Context::TriggerGC() {
            ENGINE_PROFILER_SCOPE;
            
            v8::Isolate *isolate = v8::Isolate::GetCurrent();
            
            isolate->IdleNotification(1000);
            platform->PumpMessages(isolate);
        }
        
        // Globals
        
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
            ENGINE_PROFILER_SCOPE;
            return _getValueFromV8Object(obj);
        }
        
        v8::Local<v8::Value> _getValueFromJson(v8::Isolate* isolate, Json::Value val) {
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
                        ret->Set(_getValueFromJson(isolate, iter.key()), _getValueFromJson(isolate, val[iter.key().asString()]));
                    }
                    return ret;
                }
                case Json::objectValue: {
                    v8::Local<v8::Object> ret = v8::Object::New(isolate);
                    for (auto iter = val.begin(); iter != val.end(); iter++) {
                        ret->Set(v8::String::NewFromUtf8(isolate, iter.key().asCString()), _getValueFromJson(isolate, val[iter.key().asString()]));
                    }
                    return ret;
                }
                default:
                    throw "_getValueFromJson type unimplamented";
            }
        }
        
        v8::Local<v8::Object> GetObjectFromJson(Json::Value val) {
            ENGINE_PROFILER_SCOPE;
            return _getValueFromJson(v8::Isolate::GetCurrent(), val).As<v8::Object>();
        }
    }
}
