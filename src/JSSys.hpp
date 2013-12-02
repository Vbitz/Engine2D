#pragma once

#include "Scripting.hpp"
#include "Util.hpp"

namespace Engine {

	namespace JsSys {
        ENGINE_JS_METHOD(Println);
        
        void InitSys(v8::Handle<v8::ObjectTemplate> sysTable);
	}

}