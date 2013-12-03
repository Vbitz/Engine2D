#pragma once

#include "Scripting.hpp"
#include "Util.hpp"

namespace Engine {

	namespace JsSys {
        ENGINE_JS_METHOD(Println);
        ENGINE_JS_METHOD(ClearConsole);
        
        void InitSys(v8::Handle<v8::ObjectTemplate> sysTable);
	}

}