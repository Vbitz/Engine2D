#include "JSSys.hpp"

namespace Engine {

	namespace JsSys {

		ENGINE_JS_METHOD(Println) {
			ENGINE_JS_SCOPE_OPEN;
            
			bool first = true;
			for (int i = 0; i < args.Length(); i++) {
				if (first) {
					first = false;
				} else {
					printf(" ");
				}
				v8::String::Utf8Value str(args[i]);
				const char* cstr = *str;
				printf("%s", cstr);
			}
			printf("\n");
	
			fflush(stdout);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}

        ENGINE_JS_METHOD(RunFile) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_STRING(0);
            ENGINE_CHECK_ARG_BOOLEAN(1);
            
            std::string scriptFilename = *ENGINE_GET_ARG_CSTRING_VALUE(0) + std::string(".js");
			runFile(scriptFilename, ENGINE_GET_ARG_BOOLEAN_VALUE(1));
			
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
		
        ENGINE_JS_METHOD(Drawfunc) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_FUNCTION(0);
            
			v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args[0]);
            
			setDrawFunction(v8::Persistent<v8::Function>::New(v8::Isolate::GetCurrent(), func));
            
			ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
        
        ENGINE_JS_METHOD(KeyboardFunc) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_FUNCTION(0);
            
			v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args[0]);
            
			setKeyFunction(v8::Persistent<v8::Function>::New(v8::Isolate::GetCurrent(), func));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(GetGLVersion) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Handle<v8::Object> ret = v8::Object::New();
            
            int glMajor, glMinor, glRev;
            
            glfwGetGLVersion(&glMajor, &glMinor, &glRev);
            
            ret->Set(v8::String::NewSymbol("major"), v8::Number::New(glMajor));
            ret->Set(v8::String::NewSymbol("minor"), v8::Number::New(glMinor));
            ret->Set(v8::String::NewSymbol("rev"), v8::Number::New(glRev));
            
            ENGINE_JS_SCOPE_CLOSE(ret);
        }
        
        ENGINE_JS_METHOD(HasExtention) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0);
            
            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(glewGetExtension(*ENGINE_GET_ARG_CSTRING_VALUE(0))));
        }
        
        ENGINE_JS_METHOD(SaveScreenshot) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0);
            
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
            
            ENGINE_CHECK_ARG_INT32(0);
            ENGINE_CHECK_ARG_INT32(1);
            
            glfwSetWindowSize(ENGINE_GET_ARG_INT32_VALUE(0),
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
        
        ENGINE_JS_METHOD(ReadFile) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0);
            
            std::string path = ENGINE_GET_ARG_CPPSTRING_VALUE(0);
            
            if (!Filesystem::FileExists(path)) {
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            ENGINE_JS_SCOPE_CLOSE(v8::String::New(Filesystem::GetFileContent(path)));
        }
        
        ENGINE_JS_METHOD(FileExists) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0);
            
            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(Filesystem::FileExists(ENGINE_GET_ARG_CPPSTRING_VALUE(0))));
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

	}

}