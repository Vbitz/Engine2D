#include "Util.hpp"

namespace engine {

    bool _hasGLContext = false;

    bool HasGLContext() {
        return _hasGLContext;
    }

    void EnableGLContext() {
        _hasGLContext = true;
    }

    void DisableGLContext() {
        _hasGLContext = false;
    }

}