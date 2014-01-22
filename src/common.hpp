#ifndef __COMMONH
#define __COMMONH

// system includes
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <map>

#include "Scripting.hpp"

#include <sys/stat.h>

// system includes
#ifdef _WIN32
    #include <GL\glew.h>
    #include <GLFW\glfw3.h>
	#include <sys\timeb.h>
	#include <Windows.h>
    #define _PLATFORM "Windows"
    #define _PLATFORM_DYLINK ".dll"
	#define _PLATFORM_WIN32
#else
    #include <GL/glew.h>
    #include <GLFW/glfw3.h>
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

#endif