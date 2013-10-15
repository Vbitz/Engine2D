#include "JSSys.hpp"

#include "LogGraphEvents.hpp"

namespace Engine {

	namespace JsSys {
        
        Logger::LogLevel _getLevelFromStr(std::string str) {
            if (str == "warning") return Logger::LogLevel_Warning;
            if (str == "verbose") return Logger::LogLevel_Verbose;
            if (str == "error") return Logger::LogLevel_Error;
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
            
            Logger::LogText("Script", _getLevelFromStr(ENGINE_GET_ARG_CPPSTRING_VALUE(0)), logStr);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
        
        ENGINE_JS_METHOD(ArgV) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(0);
            
            v8::Local<v8::Array> argvArray = v8::Array::New();
            
            int argc = getCommandLineArgCount();
            const char** argv = getCommandLineArgs();
            
            for (int i = 0; i < argc; i++) {
                argvArray->Set(i, v8::String::New(argv[i]));
            }
            
            ENGINE_JS_SCOPE_CLOSE(argvArray);
        }

        ENGINE_JS_METHOD(RunFile) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the filename of the script to load");
            ENGINE_CHECK_ARG_BOOLEAN(1, "Arg1 is set to automaticly reload Arg0 when it's changed");
            
            std::string scriptFilename = *ENGINE_GET_ARG_CSTRING_VALUE(0) + std::string(".js");
			runFile(scriptFilename, ENGINE_GET_ARG_BOOLEAN_VALUE(1));
			
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
		
        ENGINE_JS_METHOD(Drawfunc) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_FUNCTION(0, "Arg0 is a function to be called each time a frame is to be rendered");
            
			v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args[0]);
            
			setDrawFunction(v8::Persistent<v8::Function>::New(v8::Isolate::GetCurrent(), func));
            
			ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
        
        ENGINE_JS_METHOD(KeyboardFunc) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_FUNCTION(0, "Arg0 is a function to be called on each key pressed or released");
            
			v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args[0]);
            
			setKeyFunction(v8::Persistent<v8::Function>::New(v8::Isolate::GetCurrent(), func));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(OnPostLoad) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_FUNCTION(0, "Arg0 is a function to be called once the opengl context is created");
            
			v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args[0]);
            
			setPostLoadFunction(v8::Persistent<v8::Function>::New(v8::Isolate::GetCurrent(), func));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(GetGLVersion) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Handle<v8::Object> ret = v8::Object::New();
            
            ret->Set(v8::String::NewSymbol("major"), v8::Number::New(glfwGetWindowAttrib(getGLFWWindow(), GLFW_CONTEXT_VERSION_MAJOR)));
            ret->Set(v8::String::NewSymbol("minor"), v8::Number::New(glfwGetWindowAttrib(getGLFWWindow(), GLFW_CONTEXT_VERSION_MINOR)));
            ret->Set(v8::String::NewSymbol("rev"), v8::Number::New(glfwGetWindowAttrib(getGLFWWindow(), GLFW_CONTEXT_REVISION)));
            
            ENGINE_JS_SCOPE_CLOSE(ret);
        }
        
        ENGINE_JS_METHOD(HasExtention) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the name of a GL Extention to check for");
            
            bool has = glewGetExtension(*ENGINE_GET_ARG_CSTRING_VALUE(0));
            
            Draw2D::CheckGLError("Post Has Extention");
            
            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(has));
        }
        
        ENGINE_JS_METHOD(GetExtentions) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            v8::Handle<v8::Array> arr = v8::Array::New();
            
            if (usingGL3()) {
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
        
        ENGINE_JS_METHOD(SaveScreenshot) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the filename to save the screenshot as");
            
            saveScreenshot(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(Microtime) {
            ENGINE_JS_SCOPE_OPEN;
            
#ifdef _PLATFORM_WIN32
			ENGINE_JS_SCOPE_CLOSE(v8::Number::New(GetTickCount()));
#else
			timeval time;
            gettimeofday(&time, NULL);

            ENGINE_JS_SCOPE_CLOSE(v8::Number::New(time.tv_sec + (time.tv_usec * 0.000001)));
#endif
        }
        
        ENGINE_JS_METHOD(ResizeWindow) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_INT32(0, "Arg0 is the new X size of the window");
            ENGINE_CHECK_ARG_INT32(1, "Arg1 is the new Y size of the window");
            
            glfwSetWindowSize(getGLFWWindow(),
                              ENGINE_GET_ARG_INT32_VALUE(0),
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
        
        ENGINE_JS_METHOD(ToggleFullscreen) {
            ENGINE_JS_SCOPE_OPEN;
            
            toggleFullscreen();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(RestartRenderer) {
            ENGINE_JS_SCOPE_OPEN;
            
            restartRenderer();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(Exit) {
            ENGINE_JS_SCOPE_OPEN;
            
            exit();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(PerfZone) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the name of the Profile Zone");
            ENGINE_CHECK_ARG_FUNCTION(1, "Arg1 is the function to profile");
            
            Profiler::Begin(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            
            v8::Context::Scope ctx_scope(getGlobalContext());
            
            v8::Handle<v8::Function> real_func = v8::Handle<v8::Function>::Cast(args[1]);
            
            v8::TryCatch tryCatch;
            
            real_func->Call(getGlobalContext()->Global(), 0, NULL);
            
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
            
            std::vector<std::string> zones = Profiler::GetZones();
            
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
            
            double startTime = Logger::GetTime();
            
            v8::Context::Scope ctx_scope(getGlobalContext());
            
            v8::Handle<v8::Function> real_func = v8::Handle<v8::Function>::Cast(args[1]);
            
            v8::TryCatch tryCatch;
            
            real_func->Call(getGlobalContext()->Global(), 0, NULL);
            
            if (!tryCatch.Exception().IsEmpty()) {
                v8::ThrowException(tryCatch.Exception());
            }
            
            Logger::begin("JSSys", Logger::LogLevel_User) << "Timed: " << ENGINE_GET_ARG_CPPSTRING_VALUE(0) << " at "
                << (Logger::GetTime() - startTime) << "s" << Logger::end();
            
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
            
            detailProfile(ENGINE_GET_ARG_INT32_VALUE(0), ENGINE_GET_ARG_CPPSTRING_VALUE(1));
            
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
        
        ENGINE_JS_METHOD(ReloadRootScript) {
            ENGINE_JS_SCOPE_OPEN;
            
            invalidateScript(Config::GetString("script_bootloader"));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        };
        
        ENGINE_JS_METHOD(ForceReload) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the script to reload");
            
            invalidateScript(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        };
        
        ENGINE_JS_METHOD(TestGraph) {
            ENGINE_JS_SCOPE_OPEN;
            
            Logger::LogGraph("Testing", Logger::LogLevel_Log, new LogGraphEvents::TestingEvent());
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
#define addItem(table, js_name, funct) table->Set(js_name, v8::FunctionTemplate::New(funct))
        
        void InitSys(v8::Handle<v8::ObjectTemplate> sysTable) {
			addItem(sysTable, "println", Println);
            
            addItem(sysTable, "argv", ArgV);
            
			addItem(sysTable, "runFile", RunFile);
            
			addItem(sysTable, "drawFunc", Drawfunc);
            addItem(sysTable, "keyboardFunc", KeyboardFunc);
            addItem(sysTable, "onPostLoad", OnPostLoad);
            
            addItem(sysTable, "microtime", Microtime);
            
            addItem(sysTable, "heapStats", HeapStats);
            
            addItem(sysTable, "exit", Exit);
            
            addItem(sysTable, "getGLVersion", GetGLVersion);
            addItem(sysTable, "hasExtention", HasExtention);
            addItem(sysTable, "getExtentions", GetExtentions);
            addItem(sysTable, "getMaxTextureSize", GetMaxTextureSize);
            
            addItem(sysTable, "saveScreenshot", SaveScreenshot);
            
            addItem(sysTable, "resizeWindow", ResizeWindow);
            addItem(sysTable, "toggleFullscreen", ToggleFullscreen);
            addItem(sysTable, "restartRenderer", RestartRenderer);
            
            addItem(sysTable, "getProfilerTime", GetProfilerTime);
            addItem(sysTable, "getProfilerZones", GetProfileZones);
            
            addItem(sysTable, "perf", PerfZone);
            addItem(sysTable, "time", TimeZone);
            
            addItem(sysTable, "config", ConfigOptions);
            
            addItem(sysTable, "gc", GC);
            addItem(sysTable, "profile", Profile);
            
            addItem(sysTable, "profileSet", ProfileSet);
            
            addItem(sysTable, "clearConsole", ClearConsole);
            
            addItem(sysTable, "reloadRootScript", ReloadRootScript);
            addItem(sysTable, "forceReload", ForceReload);
            
            //addItem(sysTable, "testGraph", TestGraph);
        }
        
#undef addItem

	}

}