#pragma once

#include "common.hpp"
#include "Main.hpp"

namespace Engine {
    
	namespace JsInput {
		v8::Handle<v8::Value>		KeyDown(const v8::Arguments& args);
	}
    
}