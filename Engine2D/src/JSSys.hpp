#pragma once

#include "common.hpp"
#include "Main.hpp"

namespace Engine {

	namespace JsSys {

		ENGINE_JS_METHOD(Println);
        
		ENGINE_JS_METHOD(RunFile);

		ENGINE_JS_METHOD(Drawfunc);
        ENGINE_JS_METHOD(KeyboardFunc);
        
        ENGINE_JS_METHOD(GetGLVersion);
        ENGINE_JS_METHOD(HasExtention);
        
        ENGINE_JS_METHOD(Microtime);
        
        ENGINE_JS_METHOD(ResizeWindow);
        
        ENGINE_JS_METHOD(HeapStats);
        
        ENGINE_JS_METHOD(ReadFile);
        ENGINE_JS_METHOD(MountFile);
        
        ENGINE_JS_METHOD(ToggleFullscreen);
        
        ENGINE_JS_METHOD(Exit);
	}

}