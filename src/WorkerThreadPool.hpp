#pragma once

#include "Platform.hpp"

#include <string>

namespace Engine {
    namespace WorkerThreadPool {
        void CreateScriptWorker(std::string scriptSource);
    }
}