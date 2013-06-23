#pragma once

#include "common.hpp"
#include "main.hpp"

namespace Engine {

	namespace JsSys {
        ENGINE_JS_METHOD(Println);
        
        void InitSys(v8::Handle<v8::ObjectTemplate> sysTable);
	}

}