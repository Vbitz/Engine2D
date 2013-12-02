#pragma once

#include "Scripting.hpp"

namespace Engine {
    
	namespace JsInput {
		void InitInput(v8::Handle<v8::ObjectTemplate> inputTable);
	}
    
}