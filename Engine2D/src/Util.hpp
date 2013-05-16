#pragma once

#ifdef _WIN32
#include <windows.h>
#define sleep Sleep
#else
#include <unistd.h>
#endif

#include "common.hpp"

namespace Engine {
    inline bool CheckGLError(std::string source) {
        GLenum err;
        bool oneError = false;
        while ((err = glGetError()) != GL_NO_ERROR) {
            oneError = true;
            std::cerr << "GLError in " << source << std::endl;
            if (err == GL_INVALID_ENUM) {
                std::cerr << "INVALID ENUM!" << std::endl;
            } else if (err == GL_INVALID_VALUE) {
                std::cerr << "Invalid value!" << std::endl;
            } else if (err == GL_INVALID_OPERATION) {
                std::cerr << "Invalid operation!" << std::endl;
            } else if (err == GL_OUT_OF_MEMORY) {
                std::cerr << "Out of Memory" << std::endl;
            }
        }
        return oneError;
    }
}