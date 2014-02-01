#pragma once

#include "Scripting.hpp"

namespace Engine {

	namespace JsUnsafe {
        void InitUnsafe(v8::Handle<v8::ObjectTemplate> sysTable);
	}

}