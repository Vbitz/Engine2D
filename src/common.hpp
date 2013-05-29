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
#include <v8.h>

// system includes
#ifdef _WIN32
    #include <GL\glew.h>
    #include <GL\glfw.h>
	#include <sys\timeb.h>
	#include <Windows.h>
    #define _PLATFORM "Windows"
	#define _PLATFORM_WIN32
#else
    #include <GL/glew.h>
    #include <GL/glfw.h>
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

#include <FreeImage.h>

#include "extern/GLFT_Font.hpp"

#include "Util.hpp"

#endif