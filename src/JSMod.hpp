#pragma once

#include "common.hpp"

namespace Engine {
    
	namespace JSMod {
        
		void InitMod(v8::Handle<v8::ObjectTemplate> obj);
        
        void CloseAllOpenModules();
	}
    
}
