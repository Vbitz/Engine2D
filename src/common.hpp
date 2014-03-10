/*
   Filename: common.hpp
   Purpose:  "Common" header files

   Part of Engine2D

   Copyright (C) 2014 Vbitz

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#pragma once

// system includes
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <map>

#include "Scripting.hpp"

#include <sys/stat.h>

#include "Window.hpp"

// system includes
#ifdef _WIN32
	#include <sys\timeb.h>
	#include <Windows.h>
    #define _PLATFORM "Windows"
    #define _PLATFORM_DYLINK ".dll"
	#define _PLATFORM_WIN32
#else
    #include <sys/time.h>
    #ifdef __APPLE__
        #ifdef __MACH__
            #define _PLATFORM "Darwin (OSX)"
            #define _PLATFORM_DYLINK ".dylib"
			#define _PLATFORM_OSX
        #else
            #error Unknown Platform
        #endif
    #elif __linux__
        #define _PLATFORM "Linux"
        #define _PLATFORM_DYLINK ".so"
		#define _PLATFORM_LINUX
    #else
        #error Unknown Platform
    #endif
#endif

#define STR(val) #val

// TODO: define GLM_FORCE_RADIANS, I need to make sure that Draw2D.rotateCamera does this
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine {
    namespace EffectShaderTypes {
        enum Type {
            GLSL_150,
            Unknown
        };
    };
};

#include "Logger.hpp"

#include "Profiler.hpp"

#include "Util.hpp"

#include "Config.hpp"

#include "Platform.hpp"

#include "Events.hpp"