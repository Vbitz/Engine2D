#ifndef __COMMONH
#define __COMMONH

// system includes
#include <stdio.h>
#include <stdlib.h>
#include <map>

#include <sys/stat.h>

// scripting includes
#include <v8.h>

// opengl includes
#ifdef _WIN32
#include <GL\glfw.h>
#else
#include <GL/glfw.h>
#endif

#include "extern/GLFT_Font.hpp"

#endif