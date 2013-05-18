#ifndef __COMMONH
#define __COMMONH

// system includes
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>

#include <sys/stat.h>

// scripting includes
#include <v8.h>

// system includes
#ifdef _WIN32
    #include <GL\glew.h>
    #include <GL\glfw.h>
    #define _PLATFORM "Windows"
#else
    #include <GL/glew.h>
    #include <GL/glfw.h>
    #include <sys/time.h>
    #ifdef __APPLE__
        #ifdef __MACH__
            #define _PLATFORM "Darwin (OSX)"
        #else
            #error Unknown Platform
        #endif
    #elif __linux__
        #define _PLATFORM "Linux"
    #else
        #error Unknown Platform
    #endif
#endif

#include "extern/GLFT_Font.hpp"

#include "Util.hpp"

#endif