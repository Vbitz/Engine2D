#include "JSSys.hpp"

namespace Engine {

	namespace JsSys {

		v8::Handle<v8::Value> Println(const v8::Arguments& args) {
			v8::HandleScope handle_scope;
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
			return handle_scope.Close(v8::Undefined());
		}

		v8::Handle<v8::Value> RunFile(const v8::Arguments& args) {
			v8::HandleScope handle_scope;
			if (args.Length() != 2) {
				return handle_scope.Close(v8::Undefined());
			}
			if (!args[0]->IsString() || !args[1]->IsBoolean()) {
				return handle_scope.Close(v8::Undefined());
			}
			runFile(*v8::String::AsciiValue(args[0]), args[1]->ToBoolean()->Value());
			return handle_scope.Close(v8::Undefined());
		}
        
        v8::Handle<v8::Value> KeyboardFunc(const v8::Arguments& args) {
            v8::HandleScope handle_scope;
            
            if (args.Length() != 1) {
				// TODO : Error
				return handle_scope.Close(v8::Undefined());
			}
            
			if (!args[0]->IsFunction()) {
				// TODO : handle_scope
				return handle_scope.Close(v8::Undefined());
			}
            
			v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args[0]);
            
			setKeyFunction(v8::Persistent<v8::Function>::New(v8::Isolate::GetCurrent(), func));
            
            return handle_scope.Close(v8::Undefined());
        }
        
        v8::Handle<v8::Value> GetGLVersion(const v8::Arguments& args) {
            v8::HandleScope scope;
            v8::Handle<v8::Object> ret = v8::Object::New();
            
            int glMajor, glMinor, glRev;
            
            glfwGetGLVersion(&glMajor, &glMinor, &glRev);
            
            ret->Set(v8::String::New("major"), v8::Number::New(glMajor));
            ret->Set(v8::String::New("minor"), v8::Number::New(glMinor));
            ret->Set(v8::String::New("rev"), v8::Number::New(glRev));
            
            return scope.Close(ret);
        }
        
        v8::Handle<v8::Value> Microtime(const v8::Arguments& args) {
            v8::HandleScope scope;
            
            timeval time;
            gettimeofday(&time, NULL);
            
            return scope.Close(v8::Number::New(time.tv_sec + (time.tv_usec * 0.000001)));
        }
        
        v8::Handle<v8::Value> HeapStats(const v8::Arguments& args) {
            v8::HandleScope scope;
            v8::Handle<v8::Object> ret = v8::Object::New();
            
            v8::HeapStatistics stats;
            
            v8::Isolate::GetCurrent()->GetHeapStatistics(&stats);
            
            ret->Set(v8::String::NewSymbol("heapLimit"), v8::Number::New(stats.heap_size_limit()));
            ret->Set(v8::String::NewSymbol("heapTotalSize"), v8::Number::New(stats.total_heap_size()));
            ret->Set(v8::String::NewSymbol("heapTotalExecSize"), v8::Number::New(stats.total_heap_size_executable()));
            ret->Set(v8::String::NewSymbol("heapUsed"), v8::Number::New(stats.used_heap_size()));
            
            return scope.Close(ret);
        }

	}

}