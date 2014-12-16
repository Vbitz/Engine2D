/*
   Filename: JSSys.cpp
   Purpose:  Various functions to support Javascript and bind to C++ APIs

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

#include "../JSSys.hpp"

#include "../Application.hpp"
#include "../EngineUI.hpp"
#include "../Timer.hpp"
#include "../WorkerThreadPool.hpp"
#include "../Package.hpp"

#include "../RenderDriver.hpp"

#include "../Config.hpp"
#include "../Profiler.hpp"

namespace Engine {

	namespace JsSys {
        
        Logger::LogLevel _getLevelFromStr(std::string str) {
                 if (str == "warning")      return Logger::LogLevel_Warning;
            else if (str == "verbose")      return Logger::LogLevel_Verbose;
            else if (str == "error")        return Logger::LogLevel_Error;
            else if (str == "highlight")    return Logger::LogLevel_Highlight;
            else if (str == "toast")        return Logger::LogLevel_Toast;
            else                            return Logger::LogLevel_User;
        }
        
        ApplicationPtr GetApp(v8::Local<v8::Object> thisValue) {
            return (ApplicationPtr) thisValue->GetHiddenValue(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "_app")).As<v8::External>()->Value();
        }

        class JS_Package : public ScriptingManager::ObjectWrap {
        public:
            
            ~JS_Package() {
                delete _pkg;
            }

            static void New(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                if (args.RecallAsConstructor()) return;
                if (args.AssertCount(1)) return;
                if (args.Assert(args[0]->IsString(), "Arg0 is the filename to open")) return;
                
                JS_Package* jsPkg = Wrap<JS_Package>(args.GetIsolate(), args.This());

                jsPkg->_pkg = Package::FromFile(args.StringValue(0));
            }

            static void ReadFile(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);

                if (args.AssertCount(1)) return;
                if (args.Assert(args[0]->IsString(), "Arg0 is the filename to read")) return;

                uint32_t fileSize = 0;

                uint8_t *fileData = Unwrap<JS_Package>(args.This())->_pkg->ReadFile(args.StringValue(0), fileSize);

                // TODO: Support decoding as array
                std::string fileString = std::string((char*) fileData, (size_t) fileSize);

                args.SetReturnValue(args.NewString(fileString)); // Sigh
            }

            static void FileExists(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);

                if (args.AssertCount(1)) return;
                if (args.Assert(args[0]->IsString(), "Arg0 is the filename to read")) return;

                args.SetReturnValue(args.NewBoolean(Unwrap<JS_Package>(args.This())->_pkg->FileExists(args.StringValue(0))));
            }

            static void FromJsonSpec(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);

                if (args.AssertCount(2)) return;
                if (args.Assert(args[0]->IsString(), "Arg0 is the source manifest to read") ||
                    args.Assert(args[1]->IsString(), "Arg0 is the destintaiton package filename to write")) return;

                PackagePtr pkg = Package::FromJsonSpec(args.StringValue(0), args.StringValue(1));

                delete pkg;
            }
            
            static void Init(v8::Isolate* isolate, v8::Handle<v8::ObjectTemplate> sys_table) {
                ScriptingManager::Factory f(isolate);
                
                v8::Handle<v8::FunctionTemplate> newPackage = f.NewFunctionTemplate(New);
                
                newPackage->SetClassName(f.NewString("Package"));
                
                f.FillTemplate(newPackage, {
                    {FTT_Prototype, "readFile", f.NewFunctionTemplate(ReadFile)},
                    {FTT_Prototype, "fileExists", f.NewFunctionTemplate(FileExists)},
                    {FTT_Static, "fromJsonSpec", f.NewFunctionTemplate(FromJsonSpec)},
                });
                
                newPackage->InstanceTemplate()->SetInternalFieldCount(1);
                
                sys_table->Set(f.NewString("Package"), newPackage);
            }

        private:
            PackagePtr _pkg = NULL;
        };
        
        void Println(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            std::string logStr = "";
            
			bool first = true;
			for (int i = 1; i < args.Length(); i++) {
				if (first) {
					first = false;
				} else {
					logStr += " ";
				}
				v8::String::Utf8Value str(args[i]->ToString());
				const char* cstr = *str;
				logStr += cstr;
			}
            
            if (args.StringValue(0) == "raw") {
                printf("%s", logStr.c_str());
            } else {
                Logger::LogText("Script", _getLevelFromStr(args.StringValue(0)), logStr);
            }
		}
        
        void ArgV(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            v8::Local<v8::Array> argvArray = args.NewArray();
            
            std::vector<std::string> cArgs = GetApp(args.This())->GetCommandLineArgs();
            
            for (int i = 0; i < cArgs.size(); i++) {
                argvArray->Set(i, v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), cArgs[i].c_str()));
            }
            
            args.SetReturnValue(argvArray);
        }

        void RunFile(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(2)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the filename of the script to load") ||
                args.Assert(args[1]->IsBoolean(), "Arg1 is set to automaticly reload Arg0 when it's changed")) return;
			
            args.SetReturnValue(args.NewBoolean(GetApp(args.This())->GetScriptingContext()->RunFile(args.StringValue(0), args.BooleanValue(1))));
		}
        
        void EventsOn(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Length() == 3) {
                GetEventsSingilton()->GetEvent(args.StringValue(0))->AddListener(args.StringValue(1), EventEmitter::MakeTarget(args[2].As<v8::Function>()));
            } else {
                
            }
        }
        
        void EventsEmit(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the Event name to Emit")) return;
            
            if (args.Length() == 2) {
                GetEventsSingilton()->GetEvent(args.StringValue(0))->Emit(ScriptingManager::ObjectToJson(v8::Handle<v8::Object>(args[1].As<v8::Object>())));
            } else if (args.Length() == 1) {
                GetEventsSingilton()->GetEvent(args.StringValue(0))->Emit();
            } else {
                args.ThrowArgError("sys.emit takes 1 or 2 args");
            }
        }
        
        void EventsSetDefered(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(2)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the Event name to target") ||
                args.Assert(args[1]->IsBoolean(), "Arg1 is set to defer messages sent to the event")) return;
            
            GetEventsSingilton()->GetEvent(args.StringValue(0))->SetDefered(args.BooleanValue(1));
        }
        
        ENGINE_JS_METHOD(EventsPollDeferedMessages) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the Event name to target");
            
            GetEventsSingilton()->GetEvent(ENGINE_GET_ARG_CPPSTRING_VALUE(0))->PollDeferedMessages();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(CreateTimer) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the time interval in seconds before triggering the timer");
            ENGINE_CHECK_ARG_STRING(1, "Arg1 is the Event to trigger");
            
            unsigned int ret = UINT_MAX;
            
            if (args.Length() == 2) {
                ret = Timer::Create(ENGINE_GET_ARG_NUMBER_VALUE(0),
                              ENGINE_GET_ARG_CPPSTRING_VALUE(1));
            } else if (args.Length() == 3) {
                ENGINE_CHECK_ARG_BOOLEAN(2, "Arg2 causes the timer to repeat if true");
                ret = Timer::Create(ENGINE_GET_ARG_NUMBER_VALUE(0),
                              ENGINE_GET_ARG_CPPSTRING_VALUE(1),
                              ENGINE_GET_ARG_BOOLEAN_VALUE(2));
            } else {
                ENGINE_THROW_ARGERROR("sys.createTimer takes 2 or 3 arguments");
            }
            
            ENGINE_JS_SCOPE_CLOSE(v8::Number::New(args.GetIsolate(), ret));
        }
        
        ENGINE_JS_METHOD(DeleteTimer) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the TimerID to Remove");
            
            Timer::Remove(ENGINE_GET_ARG_INT32_VALUE(0));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(EventsClear) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the eventID to clear");
            
            GetEventsSingilton()->Clear(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(GetGLVersion) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Isolate* isolate = args.GetIsolate();
            
            v8::Handle<v8::Object> ret = v8::Object::New(isolate);
            
            OpenGLVersion version = GetApp(args.This())->GetRender()->GetOpenGLVersion();

            ret->Set(v8::String::NewFromUtf8(isolate, "major"), v8::Number::New(isolate, version.major));
            ret->Set(v8::String::NewFromUtf8(isolate, "minor"), v8::Number::New(isolate, version.minor));
            ret->Set(v8::String::NewFromUtf8(isolate, "rev"), v8::Number::New(isolate, version.revision));
            
            ENGINE_JS_SCOPE_CLOSE(ret);
        }
        
        ENGINE_JS_METHOD(HasExtention) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the name of a GL Extention to check for");
            
            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(args.GetIsolate(), GetApp(args.This())->GetRender()->HasExtention(ENGINE_GET_ARG_CPPSTRING_VALUE(0))));
        }
        
        ENGINE_JS_METHOD(GetExtentions) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Isolate* isolate = args.GetIsolate();
            
            ENGINE_CHECK_GL;
            
            v8::Handle<v8::Array> arr = v8::Array::New(isolate);
            
            std::vector<std::string> extentionList = GetApp(args.This())->GetRender()->GetExtentions();
            
            for (int i = 0; i < extentionList.size(); i++) {
                arr->Set(i, v8::String::NewFromUtf8(isolate, extentionList[i].c_str()));
            }
            
            ENGINE_JS_SCOPE_CLOSE(arr);
        }
        
        ENGINE_JS_METHOD(GetMaxTextureSize) {
            ENGINE_JS_SCOPE_OPEN;

            ApplicationPtr app = GetApp(args.This());

            if (app->IsHeadlessMode()) {
                ENGINE_JS_SCOPE_CLOSE(v8::Integer::New(args.GetIsolate(), 0));
            } else {
                ENGINE_JS_SCOPE_CLOSE(v8::Integer::New(args.GetIsolate(), app->GetWindow()->GetMaxTextureSize()));
            }
        }
        
        ENGINE_JS_METHOD(Microtime) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_JS_SCOPE_CLOSE(v8::Number::New(args.GetIsolate(), Platform::GetTime()));
        }
        
        ENGINE_JS_METHOD(ResizeWindow) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_INT32(0, "Arg0 is the new X size of the window");
            ENGINE_CHECK_ARG_INT32(1, "Arg1 is the new Y size of the window");

            ApplicationPtr app = GetApp(args.This());

            if (!app->IsHeadlessMode()) {
                app->GetWindow()->SetWindowSize(glm::vec2(   ENGINE_GET_ARG_INT32_VALUE(0), ENGINE_GET_ARG_INT32_VALUE(1)));
            }

            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(HeapStats) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Isolate* isolate = args.GetIsolate();
            
            v8::Handle<v8::Object> ret = v8::Object::New(isolate);
            
            v8::HeapStatistics stats;
            
            v8::Isolate::GetCurrent()->GetHeapStatistics(&stats);
            
            ret->Set(v8::String::NewFromUtf8(isolate, "heapLimit"), v8::Number::New(isolate, stats.heap_size_limit()));
            ret->Set(v8::String::NewFromUtf8(isolate, "heapTotalSize"), v8::Number::New(isolate, stats.total_heap_size()));
            ret->Set(v8::String::NewFromUtf8(isolate, "heapTotalExecSize"), v8::Number::New(isolate, stats.total_heap_size_executable()));
            ret->Set(v8::String::NewFromUtf8(isolate, "heapUsed"), v8::Number::New(isolate, stats.used_heap_size()));
            
            ENGINE_JS_SCOPE_CLOSE(ret);
        }
        
        ENGINE_JS_METHOD(MemoryStats) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Isolate* isolate = args.GetIsolate();
            
            v8::Handle<v8::Object> ret = v8::Object::New(isolate);
            
            Platform::engine_memory_info mem_info = Platform::GetMemoryInfo();
            
            ret->Set(v8::String::NewFromUtf8(isolate, "totalVirutal"), v8::Number::New(isolate, mem_info.totalVirtual));
            ret->Set(v8::String::NewFromUtf8(isolate, "totalVirtualFree"), v8::Number::New(isolate, mem_info.totalVirtualFree));
            ret->Set(v8::String::NewFromUtf8(isolate, "myVirtualUsed"), v8::Number::New(isolate, mem_info.myVirtualUsed));
            ret->Set(v8::String::NewFromUtf8(isolate, "totalPhysical"), v8::Number::New(isolate, mem_info.totalPhysical));
            ret->Set(v8::String::NewFromUtf8(isolate, "totalPhysicalFree"), v8::Number::New(isolate, mem_info.totalPhysicalFree));
            ret->Set(v8::String::NewFromUtf8(isolate, "myPhysicalUsed"), v8::Number::New(isolate, mem_info.myPhysicalUsed));
            
            ENGINE_JS_SCOPE_CLOSE(ret);
        }
        
        ENGINE_JS_METHOD(Trace) {
            ENGINE_JS_SCOPE_OPEN;
            
            Platform::DumpStackTrace();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(Exit) {
            ENGINE_JS_SCOPE_OPEN;
            
            GetApp(args.This())->Exit();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(TimeZone) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the name of the zone to time");
            ENGINE_CHECK_ARG_FUNCTION(1, "Arg1 is the function to time");
            
            double startTime = Platform::GetTime();
            
            v8::Context::Scope ctx_scope(args.GetIsolate()->GetCurrentContext());
            
            v8::Handle<v8::Function> real_func = v8::Handle<v8::Function>::Cast(args[1]);
            
            v8::TryCatch tryCatch;
            
            real_func->Call(args.GetIsolate()->GetCurrentContext()->Global(), 0, NULL);
            
            if (!tryCatch.Exception().IsEmpty()) {
                args.GetIsolate()->ThrowException(tryCatch.Exception());
            }
            
            Logger::begin("JSSys", Logger::LogLevel_User) << "Timed: " << ENGINE_GET_ARG_CPPSTRING_VALUE(0) << " at "
                << (Platform::GetTime() - startTime) << "s" << Logger::end();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(ConfigOptions) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Isolate* isolate = args.GetIsolate();
            
            if (args.Length() == 0) {
                // print all config options
                std::vector<std::string> items = Config::GetAll();
                for (auto iter = items.begin(); iter != items.end(); iter++) {
                    Logger::begin("Config", Logger::LogLevel_Log) << *iter << Logger::end();
                }
            } else if (args.Length() == 1) {
                // get config option
                ENGINE_CHECK_ARG_STRING(0, "Arg0 is the Config Key to Get");
                switch (Config::GetType(ENGINE_GET_ARG_CPPSTRING_VALUE(0))) {
                    case Config::ConfigType_Bool:
                        ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(isolate, Config::GetBoolean(ENGINE_GET_ARG_CPPSTRING_VALUE(0))));
                    case Config::ConfigType_Number:
                        ENGINE_JS_SCOPE_CLOSE(v8::Number::New(isolate, Config::GetFloat(ENGINE_GET_ARG_CPPSTRING_VALUE(0))));
                    case Config::ConfigType_String:
                        ENGINE_JS_SCOPE_CLOSE(v8::String::NewFromUtf8(isolate, Config::GetString(ENGINE_GET_ARG_CPPSTRING_VALUE(0)).c_str()));
                    default:
                        ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
                }
                ENGINE_JS_SCOPE_CLOSE(v8::String::NewFromUtf8(isolate, Config::Get(ENGINE_GET_ARG_CPPSTRING_VALUE(0)).c_str()));
            } else if (args.Length() == 2) {
                // set config option
                ENGINE_CHECK_ARG_STRING(0, "Arg0 is the Config Key to Get");
                if (args[1]->IsString()) {
                    Config::SetString(ENGINE_GET_ARG_CPPSTRING_VALUE(0), ENGINE_GET_ARG_CPPSTRING_VALUE(1));
                } else if (args[1]->IsBoolean()) {
                    Config::SetBoolean(ENGINE_GET_ARG_CPPSTRING_VALUE(0), ENGINE_GET_ARG_BOOLEAN_VALUE(1));
                } else if (args[1]->IsNumber()) {
                    Config::SetNumber(ENGINE_GET_ARG_CPPSTRING_VALUE(0), (float) ENGINE_GET_ARG_NUMBER_VALUE(1));
                } else {
                    Config::SetString(ENGINE_GET_ARG_CPPSTRING_VALUE(0), ENGINE_GET_ARG_CPPSTRING_VALUE(1));
                }
            } else {
                ENGINE_THROW_ARGERROR("sys.config accepts 0(listAll), 1(get) or 2(set) args");
            }
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(GC) {
            ENGINE_JS_SCOPE_OPEN;
            
            GetApp(args.This())->GetScriptingContext()->TriggerGC(); // recomend a full GC (the internals which I've gone through
                // say that at the moment this will recomend a full GC but the value is just a hint.)
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(ClearConsole) {
            ENGINE_JS_SCOPE_OPEN;
            
            GetApp(args.This())->GetEngineUI()->ClearConsole();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(ToggleConsole) {
            ENGINE_JS_SCOPE_OPEN;
            
            GetApp(args.This())->GetEngineUI()->ToggleConsole();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(ReloadRootScript) {
            ENGINE_JS_SCOPE_OPEN;
            
            GetApp(args.This())->GetScriptingContext()->InvalidateScript(Config::GetString("core.script.entryPoint"));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(ForceReload) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the script to reload");
            
            GetApp(args.This())->GetScriptingContext()->InvalidateScript(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        void Version(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            //if (args.Assert(HasGLContext(), "This function requies a OpemGL context")) return;
            
            v8::Handle<v8::Object> ret = args.NewObject();
            
            ApplicationPtr app = GetApp(args.This());
            
            args.FillObject(ret, {
                {FTT_Static, "v8", args.NewString(v8::V8::GetVersion())},
                {FTT_Static, "engine", args.NewString(Application::GetEngineVersion())},
            });

            if (!app->IsHeadlessMode()) {
                OpenGLVersion glVersion = app->GetWindow()->GetGlVersion();
                args.FillObject(ret, {
                    {FTT_Static, "openGL", args.NewString(glVersion.fullGLVersion)},
                    {FTT_Static, "glew", args.NewString(glVersion.glewVersion)},
                    {FTT_Static, "window", args.NewString(app->GetWindow()->GetWindowVersion())},
                    {FTT_Static, "glsl", args.NewString(glVersion.glslVersion)},
                    {FTT_Static, "glVendor", args.NewString(glVersion.glVendor)},
                    {FTT_Static, "glRenderer", args.NewString(glVersion.glRenderer)}
                });
            }
            
            args.SetReturnValue(ret);
        }
        
        ENGINE_JS_METHOD(MsgBox) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(3);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the title of the message box");
            ENGINE_CHECK_ARG_STRING(1, "Arg1 is the message in the message box");
            ENGINE_CHECK_ARG_BOOLEAN(2, "Arg2 makes the dialog modal if set to true");
            
            Platform::ShowMessageBox(ENGINE_GET_ARG_CPPSTRING_VALUE(0),
                                     ENGINE_GET_ARG_CPPSTRING_VALUE(1),
                                     ENGINE_GET_ARG_BOOLEAN_VALUE(2));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(ShellExec) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the url to execute");
            
            ENGINE_JS_SCOPE_CLOSE(v8::Number::New(args.GetIsolate(), Platform::ShellExecute(ENGINE_GET_ARG_CPPSTRING_VALUE(0))));
        }
        
        ENGINE_JS_METHOD(GetUuid) {
            ENGINE_JS_SCOPE_OPEN;
            
            Platform::UUID uuid = Platform::GenerateUUID();
            
            ENGINE_JS_SCOPE_CLOSE(v8::String::NewFromUtf8(args.GetIsolate(), Platform::StringifyUUID(uuid).c_str()));
        }
        
        ENGINE_JS_METHOD(DumpLog) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the filename to save the log as");
            
            if (!Filesystem::HasSetUserDir()) {
                ENGINE_THROW_ARGERROR("You need to call fs.configDir before you can call sys.dumpLog");
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            std::string log = Logger::DumpAllEvents();
            
            const char* logC = log.c_str();
            
            Filesystem::WriteFile(ENGINE_GET_ARG_CPPSTRING_VALUE(0), logC, log.length());
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(CreateWorker) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            WorkerThreadPool::CreateScriptWorker(std::string(*v8::String::Utf8Value(args[0]->ToString())));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(Assert) {
            ENGINE_JS_SCOPE_OPEN;
            if (GetApp(args.This())->IsDebugMode()) {
                if (args[0]->ToBoolean() == v8::False(args.GetIsolate())) {
                    if (args.Length() > 1 && args[1]->IsString()) {
                        ENGINE_ASSERT(false, ENGINE_GET_ARG_CPPSTRING_VALUE(1));
                    } else {
                        ENGINE_ASSERT(false, "");
                    }
                } else {
                    ENGINE_JS_SCOPE_CLOSE(args[0]);
                }
            } else {
                ENGINE_JS_SCOPE_CLOSE(args[0]);
            }
        }
        
#define addItem(table, js_name, funct) table->Set(isolate, js_name, v8::FunctionTemplate::New(isolate, funct))
        
        void InitSys(v8::Handle<v8::ObjectTemplate> sysTable) {
            v8::Isolate* isolate = v8::Isolate::GetCurrent();
            
            addItem(sysTable, "argv", ArgV);
            
			addItem(sysTable, "runFile", RunFile);
            
            addItem(sysTable, "on", EventsOn);
            addItem(sysTable, "emit", EventsEmit);
            addItem(sysTable, "pollDeferedMessages", EventsPollDeferedMessages);
            addItem(sysTable, "setDeferedEvent", EventsSetDefered);
            addItem(sysTable, "clearEvent", EventsClear);
            
            addItem(sysTable, "createTimer", CreateTimer);
            addItem(sysTable, "deleteTimer", DeleteTimer);
            
            addItem(sysTable, "microtime", Microtime);
            
            addItem(sysTable, "heapStats", HeapStats);
            addItem(sysTable, "memoryStats", MemoryStats);
            
            addItem(sysTable, "trace", Trace);
            addItem(sysTable, "exit", Exit);
            
            addItem(sysTable, "getGLVersion", GetGLVersion);
            addItem(sysTable, "hasExtention", HasExtention);
            addItem(sysTable, "getExtentions", GetExtentions);
            addItem(sysTable, "getMaxTextureSize", GetMaxTextureSize);
            
            addItem(sysTable, "resizeWindow", ResizeWindow);
            
            addItem(sysTable, "time", TimeZone);
            
            addItem(sysTable, "config", ConfigOptions);
            
            addItem(sysTable, "gc", GC);
            
            addItem(sysTable, "reloadRootScript", ReloadRootScript);
            addItem(sysTable, "forceReload", ForceReload);
            
            addItem(sysTable, "version", Version);
            
            addItem(sysTable, "msgBox", MsgBox);
            addItem(sysTable, "shell", ShellExec);
            addItem(sysTable, "uuid", GetUuid);
            
            addItem(sysTable, "dumpLog", DumpLog);
            
            addItem(sysTable, "createWorker", CreateWorker);

            JS_Package::Init(isolate, sysTable);
        }
        
#undef addItem

	}

}