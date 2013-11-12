#pragma once

// scripting includes
#define V8_ALLOW_ACCESS_TO_RAW_HANDLE_CONSTRUCTOR   // a massive hack right now to fix the old code
#define V8_USE_UNSAFE_HANDLES                       // this will break in the future
#include <v8.h>