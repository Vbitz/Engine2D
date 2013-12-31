#pragma once

#include "Scripting.hpp"
#include "Util.hpp"

namespace Engine {

	namespace JsUnsafe {
        void InitUnsafe(v8::Handle<v8::ObjectTemplate> sysTable);
	}

}