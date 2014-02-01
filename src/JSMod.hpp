#pragma once

#include "Scripting.hpp"

namespace Engine {
    
	namespace JSMod {
        
		void InitMod(v8::Handle<v8::ObjectTemplate> obj);
        
        void CloseAllOpenModules();
	}
    
}
