#include "JSSys.hpp"

namespace Engine {

	namespace JsSys {

		ENGINE_JS_METHOD(Println) {
			ENGINE_JS_SCOPE_OPEN;
            
            std::string logStr = "";
            
			bool first = true;
			for (int i = 0; i < args.Length(); i++) {
				if (first) {
					first = false;
				} else {
					logStr += " ";
				}
				v8::String::Utf8Value str(args[i]->ToString());
				const char* cstr = *str;
				logStr += cstr;
			}
            
            Logger::LogText("Script", Logger::LogLevel_User, logStr);
            
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
            
            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(glewGetExtension(*ENGINE_GET_ARG_CSTRING_VALUE(0))));
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
            
            addItem(sysTable, "saveScreenshot", SaveScreenshot);
            
            addItem(sysTable, "resizeWindow", ResizeWindow);
            addItem(sysTable, "toggleFullscreen", ToggleFullscreen);
            
            addItem(sysTable, "perf", PerfZone);
        }
        
#undef addItem

	}

}