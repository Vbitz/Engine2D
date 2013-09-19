#ifndef __COMMONH
#define __COMMONH

// system includes
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <map>

#include <sys/stat.h>

// scripting includes
#define V8_ALLOW_ACCESS_TO_RAW_HANDLE_CONSTRUCTOR   // a massive hack right now to fix the old code
#define V8_USE_UNSAFE_HANDLES                       // this will break in the future
#include <v8.h>

// system includes
#ifdef _WIN32
    #include <GL\glew.h>
    #include <GLFW\glfw3.h>
	#include <sys\timeb.h>
	#include <Windows.h>
    #define _PLATFORM "Windows"
	#define _PLATFORM_WIN32
#else
    #include <GL/glew.h>
    #include <GLFW/glfw3.h>
    #include <sys/time.h>
    #ifdef __APPLE__
        #ifdef __MACH__
            #define _PLATFORM "Darwin (OSX)"
			#define _PLATFORM_OSX
        #else
            #error Unknown Platform
        #endif
    #elif __linux__
        #define _PLATFORM "Linux"
		#define _PLATFORM_LINUX
    #else
        #error Unknown Platform
    #endif
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Logger.hpp"

#include "Profiler.hpp"

#include "Util.hpp"

#include "Config.hpp"

#include "ResourceManager.hpp"

#include "LogGraphEvents.hpp"

#endif