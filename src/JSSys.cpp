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

#include "JSSys.hpp"

#include "LogGraphEvents.hpp"

#include "common.hpp"
#include "Application.hpp"
#include "EngineUI.hpp"
#include "Timer.hpp"
#include "WorkerThreadPool.hpp"

namespace Engine {

	namespace JsSys {
        
        Logger::LogLevel _getLevelFromStr(std::string str) {
            if (str == "warning") return Logger::LogLevel_Warning;
            if (str == "verbose") return Logger::LogLevel_Verbose;
            if (str == "error") return Logger::LogLevel_Error;
            if (str == "highlight") return Logger::LogLevel_Highlight;
            return Logger::LogLevel_User;
        }

		ENGINE_JS_METHOD(Println) {
			ENGINE_JS_SCOPE_OPEN;
            
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
            
            if (ENGINE_GET_ARG_CPPSTRING_VALUE(0) == "raw") {
                printf("%s", logStr.c_str());
            } else {
                Logger::LogText("Script", _getLevelFromStr(ENGINE_GET_ARG_CPPSTRING_VALUE(0)), logStr);
            }
                
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
        
        ENGINE_JS_METHOD(ArgV) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(0);
            
            v8::Local<v8::Array> argvArray = v8::Array::New();
            
            std::vector<std::string> cArgs = GetAppSingilton()->GetCommandLineArgs();
            
            for (int i = 0; i < cArgs.size(); i++) {
                argvArray->Set(i, v8::String::New(cArgs[i].c_str()));
            }
            
            ENGINE_JS_SCOPE_CLOSE(argvArray);
        }

        ENGINE_JS_METHOD(RunFile) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the filename of the script to load");
            ENGINE_CHECK_ARG_BOOLEAN(1, "Arg1 is set to automaticly reload Arg0 when it's changed");
            
            std::string scriptFilename = *ENGINE_GET_ARG_CSTRING_VALUE(0) + std::string(".js");
			
            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(GetAppSingilton()->RunFile(scriptFilename, ENGINE_GET_ARG_BOOLEAN_VALUE(1))));
		}
        
        ENGINE_JS_METHOD(EventsOn) {
            ENGINE_JS_SCOPE_OPEN;
            
            if (args.Length() == 4) {
                Events::On(ENGINE_GET_ARG_CPPSTRING_VALUE(0),
                                 ENGINE_GET_ARG_CPPSTRING_VALUE(1),
                                 ScriptingManager::ObjectToJson(v8::Handle<v8::Object>(ENGINE_GET_ARG_OBJECT(2))),
                                 args[3].As<v8::Function>());
            } else if (args.Length() == 3) {
                Events::On(ENGINE_GET_ARG_CPPSTRING_VALUE(0),
                                 ENGINE_GET_ARG_CPPSTRING_VALUE(1),
                                 args[2].As<v8::Function>());
            } else {
                
            }
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(EventsEmit) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the Event name to Emit");
            
            if (args.Length() == 2) {
                Events::Emit(ENGINE_GET_ARG_CPPSTRING_VALUE(0),
                             ScriptingManager::ObjectToJson(v8::Handle<v8::Object>(args[1].As<v8::Object>())));
            } else if (args.Length() == 1) {
                Events::Emit(ENGINE_GET_ARG_CPPSTRING_VALUE(0), Json::nullValue);
            } else {
                ENGINE_THROW_ARGERROR("sys.emit takes 1 or 2 args");
            }
            
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
            
            ENGINE_JS_SCOPE_CLOSE(v8::Number::New(ret));
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
            
            Events::Clear(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(GetGLVersion) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Handle<v8::Object> ret = v8::Object::New();
            
            OpenGLVersion version = GetAppSingilton()->GetOpenGLVersion();

            ret->Set(v8::String::New("major"), v8::Number::New(version.major));
            ret->Set(v8::String::New("minor"), v8::Number::New(version.minor));
            ret->Set(v8::String::New("rev"), v8::Number::New(version.revision));
            
            ENGINE_JS_SCOPE_CLOSE(ret);
        }
        
        ENGINE_JS_METHOD(HasExtention) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the name of a GL Extention to check for");
            
            bool has = glewGetExtension(*ENGINE_GET_ARG_CSTRING_VALUE(0));
            
            if (GetAppSingilton()->UsingGL3()) {
                glGetError(); // A bug in GLEW always throws a GL error
            }
            
            Draw2D::CheckGLError("Post Has Extention");
            
            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(has));
        }
        
        ENGINE_JS_METHOD(GetExtentions) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            v8::Handle<v8::Array> arr = v8::Array::New();
            
            if (GetAppSingilton()->UsingGL3()) {
                int extentionCount;
                glGetIntegerv(GL_NUM_EXTENSIONS, &extentionCount);
            
                for (int i = 0; i < extentionCount; i++) {
                    const GLubyte* str = glGetStringi(GL_EXTENSIONS, i);
                    if (str != NULL) {
                        arr->Set(i, v8::String::New((const char*) str));
                    }
                }
            } else {
                std::string extentionList = std::string((const char*) glGetString(GL_EXTENSIONS));
                size_t currentPos = 0;
                int currentIndex = 0;
                while (currentPos < extentionList.length() && currentPos != std::string::npos) {
                    size_t nextPos = extentionList.find(' ', currentPos);
                    if (nextPos == std::string::npos) {
                        break;
                    }
                    arr->Set(currentIndex++, v8::String::New(extentionList.substr(currentPos, nextPos - currentPos).c_str()));
                    currentPos = extentionList.find(' ', currentPos) + 1;
                }
            }
            
            Draw2D::CheckGLError("GetExtentions");
            
            ENGINE_JS_SCOPE_CLOSE(arr);
        }
        
        ENGINE_JS_METHOD(GetMaxTextureSize) {
            ENGINE_JS_SCOPE_OPEN;
            
            GLint result = 0;
            
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, &result);
            
            ENGINE_JS_SCOPE_CLOSE(v8::Integer::New(result));
        }
        
        ENGINE_JS_METHOD(Microtime) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_JS_SCOPE_CLOSE(v8::Number::New(Platform::GetTime()));
        }
        
        ENGINE_JS_METHOD(ResizeWindow) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_INT32(0, "Arg0 is the new X size of the window");
            ENGINE_CHECK_ARG_INT32(1, "Arg1 is the new Y size of the window");
            
            GetAppSingilton()->SetScreenSize(   ENGINE_GET_ARG_INT32_VALUE(0),
                                                ENGINE_GET_ARG_INT32_VALUE(1));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(HeapStats) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Handle<v8::Object> ret = v8::Object::New();
            
            v8::HeapStatistics stats;
            
            v8::Isolate::GetCurrent()->GetHeapStatistics(&stats);
            
            ret->Set(v8::String::NewSymbol("heapLimit"), v8::Number::New(stats.heap_size_limit()));
            ret->Set(v8::String::NewSymbol("heapTotalSize"), v8::Number::New(stats.total_heap_size()));
            ret->Set(v8::String::NewSymbol("heapTotalExecSize"), v8::Number::New(stats.total_heap_size_executable()));
            ret->Set(v8::String::NewSymbol("heapUsed"), v8::Number::New(stats.used_heap_size()));
            
            ENGINE_JS_SCOPE_CLOSE(ret);
        }
        
        ENGINE_JS_METHOD(MemoryStats) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Handle<v8::Object> ret = v8::Object::New();
            
            Platform::engine_memory_info mem_info = Platform::GetMemoryInfo();
            
            ret->Set(v8::String::NewSymbol("totalVirutal"), v8::Number::New(mem_info.totalVirtual));
            ret->Set(v8::String::NewSymbol("totalVirtualFree"), v8::Number::New(mem_info.totalVirtualFree));
            ret->Set(v8::String::NewSymbol("myVirtualUsed"), v8::Number::New(mem_info.myVirtualUsed));
            ret->Set(v8::String::NewSymbol("totalPhysical"), v8::Number::New(mem_info.totalPhysical));
            ret->Set(v8::String::NewSymbol("totalPhysicalFree"), v8::Number::New(mem_info.totalPhysicalFree));
            ret->Set(v8::String::NewSymbol("myPhysicalUsed"), v8::Number::New(mem_info.myPhysicalUsed));
            
            ENGINE_JS_SCOPE_CLOSE(ret);
        }
        
        ENGINE_JS_METHOD(Trace) {
            ENGINE_JS_SCOPE_OPEN;
            
            Platform::DumpStackTrace();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(Exit) {
            ENGINE_JS_SCOPE_OPEN;
            
            GetAppSingilton()->Exit();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(PerfZone) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the name of the Profile Zone");
            ENGINE_CHECK_ARG_FUNCTION(1, "Arg1 is the function to profile");
            
            Profiler::Begin(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            
            v8::Context::Scope ctx_scope(v8::Context::GetCurrent());
            
            v8::Handle<v8::Function> real_func = v8::Handle<v8::Function>::Cast(args[1]);
            
            v8::TryCatch tryCatch;
            
            real_func->Call(v8::Context::GetCurrent()->Global(), 0, NULL);
            
            if (!tryCatch.Exception().IsEmpty()) {
                v8::ThrowException(tryCatch.Exception());
            }
            
            Profiler::End(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(GetProfilerTime) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the Profiler time to get");
            
            ENGINE_JS_SCOPE_CLOSE(v8::Number::New(Profiler::GetTime(ENGINE_GET_ARG_CPPSTRING_VALUE(0))));
        }
        
        ENGINE_JS_METHOD(GetProfileZones) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Handle<v8::Array> arr = v8::Array::New();
            
            std::vector<std::string> zones;
            
            if (args.Length() == 1) {
                zones = Profiler::GetZones(ENGINE_GET_ARG_BOOLEAN_VALUE(0));
            } else {
                zones = Profiler::GetZones(false);
            }
            
            for (int i = 0; i < zones.size(); i++) {
                arr->Set(i, v8::String::New(zones.at(i).c_str()));
            }
            
            ENGINE_JS_SCOPE_CLOSE(arr);
        }
        
        ENGINE_JS_METHOD(TimeZone) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the name of the zone to time");
            ENGINE_CHECK_ARG_FUNCTION(1, "Arg1 is the function to time");
            
            double startTime = Platform::GetTime();
            
            v8::Context::Scope ctx_scope(v8::Context::GetCurrent());
            
            v8::Handle<v8::Function> real_func = v8::Handle<v8::Function>::Cast(args[1]);
            
            v8::TryCatch tryCatch;
            
            real_func->Call(v8::Context::GetCurrent()->Global(), 0, NULL);
            
            if (!tryCatch.Exception().IsEmpty()) {
                v8::ThrowException(tryCatch.Exception());
            }
            
            Logger::begin("JSSys", Logger::LogLevel_User) << "Timed: " << ENGINE_GET_ARG_CPPSTRING_VALUE(0) << " at "
                << (Platform::GetTime() - startTime) << "s" << Logger::end();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(ConfigOptions) {
            ENGINE_JS_SCOPE_OPEN;
            
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
                        ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(Config::GetBoolean(ENGINE_GET_ARG_CPPSTRING_VALUE(0))));
                    case Config::ConfigType_Number:
                        ENGINE_JS_SCOPE_CLOSE(v8::Number::New(Config::GetFloat(ENGINE_GET_ARG_CPPSTRING_VALUE(0))));
                    case Config::ConfigType_String:
                        ENGINE_JS_SCOPE_CLOSE(v8::String::New(Config::GetString(ENGINE_GET_ARG_CPPSTRING_VALUE(0)).c_str()));
                    default:
                        ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
                }
                ENGINE_JS_SCOPE_CLOSE(v8::String::New(Config::Get(ENGINE_GET_ARG_CPPSTRING_VALUE(0)).c_str()));
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
            
            v8::V8::IdleNotification(200); // recomend a full GC (the internals which I've gone through
                // say that at the moment this will recomend a full GC but the value is just a hint.)
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(Profile) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Number of franes to profiler for");
            ENGINE_CHECK_ARG_STRING(1, "Filename to save report to");
            
            if (!Filesystem::HasSetUserDir()) {
                ENGINE_THROW_ARGERROR("You need to call fs.configDir before you can call sys.profile");
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            GetAppSingilton()->DetailProfile(ENGINE_GET_ARG_INT32_VALUE(0), ENGINE_GET_ARG_CPPSTRING_VALUE(1));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(ProfileSet) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_STRING(0, "Zone to set profile info on");
            
            if (args[1]->IsBoolean()) {
                if (ENGINE_GET_ARG_BOOLEAN_VALUE(1)) {
                    ENGINE_THROW_ARGERROR("Arg1 needs to be a number to enable MaxTime");
                } else {
                    Profiler::DisableMaxTime(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
                }
            } else if (args[1]->IsNumber()) {
                Profiler::EnableMaxTime(ENGINE_GET_ARG_CPPSTRING_VALUE(0),
                                        ENGINE_GET_ARG_NUMBER_VALUE(1));
            } else {
                ENGINE_THROW_ARGERROR("sys.profileSet accepts a boolean or a number for the second arg");
            }
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(ClearConsole) {
            ENGINE_JS_SCOPE_OPEN;
            
            EngineUI::ClearConsole();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(ToggleConsole) {
            ENGINE_JS_SCOPE_OPEN;
            
            EngineUI::ToggleConsole();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(ReloadRootScript) {
            ENGINE_JS_SCOPE_OPEN;
            
            GetAppSingilton()->InvalidateScript(Config::GetString("core.script.entryPoint"));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(ForceReload) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the script to reload");
            
            GetAppSingilton()->InvalidateScript(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(Version) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            v8::Handle<v8::Object> ret = v8::Object::New();
            
            std::stringstream glfwVersion;
            
            glfwVersion << GLFW_VERSION_MAJOR
                << "." << GLFW_VERSION_MINOR
                << "." << GLFW_VERSION_REVISION;
            
            ret->Set(v8::String::NewSymbol("openGL"),
                     v8::String::NewSymbol((const char*) glGetString(GL_VERSION)));
            ret->Set(v8::String::NewSymbol("glew"),
                     v8::String::NewSymbol((const char*) glewGetString(GLEW_VERSION)));
            ret->Set(v8::String::NewSymbol("v8"),
                     v8::String::NewSymbol(v8::V8::GetVersion()));
            ret->Set(v8::String::NewSymbol("engine"), v8::String::NewSymbol(Application::GetEngineVersion().c_str()));
            ret->Set(v8::String::NewSymbol("glfw"),
                     v8::String::NewSymbol(glfwVersion.str().c_str()));
            ret->Set(v8::String::NewSymbol("glsl"),
                     v8::String::NewSymbol((const char*) glGetString(GL_SHADING_LANGUAGE_VERSION)));
            
            ENGINE_JS_SCOPE_CLOSE(ret);
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
            
            ENGINE_JS_SCOPE_CLOSE(v8::Number::New(Platform::ShellExecute(ENGINE_GET_ARG_CPPSTRING_VALUE(0))));
        }
        
        ENGINE_JS_METHOD(GetUuid) {
            ENGINE_JS_SCOPE_OPEN;
            
            unsigned char* uuid = Platform::GenerateUUID();
            
            ENGINE_JS_SCOPE_CLOSE(v8::String::New(Platform::StringifyUUID(uuid).c_str()));
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
        
        ENGINE_JS_METHOD(TestGraph) {
            ENGINE_JS_SCOPE_OPEN;
            
            Logger::LogGraph("Testing", Logger::LogLevel_Log, new LogGraphEvents::TestingEvent());
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(TestAccess) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_JS_SCOPE_CLOSE(args.Data());
        }
        
        ENGINE_JS_METHOD(Assert) {
            ENGINE_JS_SCOPE_OPEN;
            if (GetAppSingilton()->IsDebugMode()) {
                if (args[0]->ToBoolean() == v8::False()) {
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
        
#define addItem(table, js_name, funct) table->Set(js_name, v8::FunctionTemplate::New(funct))
        
        void InitSys(v8::Handle<v8::ObjectTemplate> sysTable) {
            addItem(sysTable, "argv", ArgV);
            
			addItem(sysTable, "runFile", RunFile);
            
            addItem(sysTable, "on", EventsOn);
            addItem(sysTable, "emit", EventsEmit);
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
            
            addItem(sysTable, "getProfilerTime", GetProfilerTime);
            addItem(sysTable, "getProfilerZones", GetProfileZones);
            
            addItem(sysTable, "perf", PerfZone);
            addItem(sysTable, "time", TimeZone);
            
            addItem(sysTable, "config", ConfigOptions);
            
            addItem(sysTable, "gc", GC);
            addItem(sysTable, "profile", Profile);
            
            addItem(sysTable, "profileSet", ProfileSet);
            
            addItem(sysTable, "reloadRootScript", ReloadRootScript);
            addItem(sysTable, "forceReload", ForceReload);
            
            addItem(sysTable, "version", Version);
            
            addItem(sysTable, "msgBox", MsgBox);
            addItem(sysTable, "shell", ShellExec);
            addItem(sysTable, "uuid", GetUuid);
            
            addItem(sysTable, "dumpLog", DumpLog);
            
            addItem(sysTable, "createWorker", CreateWorker);
            
            //addItem(sysTable, "testAccess", TestAccess);
            
            //addItem(sysTable, "testGraph", TestGraph);
        }
        
#undef addItem

	}

}