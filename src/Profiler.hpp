#pragma once

#include <functional>

#include "Logger.hpp"

namespace Engine {
    namespace Profiler {
        void Begin(std::string zone);
        void End(std::string zone);
        
        void ProfileZone(std::string zone, std::function<void()> func);
        
        void DumpProfile();
        
        double GetTime(std::string zone);
    }
}