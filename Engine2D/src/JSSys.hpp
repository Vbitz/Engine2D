#pragma once

#include "common.hpp"
#include "Main.hpp"

namespace Engine {

	namespace JsSys {

		v8::Handle<v8::Value>		Println(const v8::Arguments& args);
        
		v8::Handle<v8::Value>		RunFile(const v8::Arguments& args);

		v8::Handle<v8::Value>		Drawfunc(const v8::Arguments& args);
        v8::Handle<v8::Value>       KeyboardFunc(const v8::Arguments& args);
        
        v8::Handle<v8::Value>       GetGLVersion(const v8::Arguments& args);
        
        v8::Handle<v8::Value>       Microtime(const v8::Arguments& args);
        v8::Handle<v8::Value>       HeapStats(const v8::Arguments& args);
        
	}

}