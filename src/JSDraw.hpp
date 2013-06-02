#pragma once

#include "common.hpp"
#include "main.hpp"

#include "Filesystem.hpp"

namespace Engine {

	namespace JsDraw {

		void Begin2d();
		void End2d();
        
        void InitDraw(v8::Handle<v8::ObjectTemplate> obj);
         
	}

}