#pragma once

#include <functional>
#include <unordered_map>
#include <sstream>
#include <map>

#include "Logger.hpp"
#include "Config.hpp"

namespace Engine {
    namespace Profiler {
        void Begin(std::string zone);
        void End(std::string zone);
        
        void DumpProfile();
        
        void ResetDetail();
        void CaptureDetail();
        std::string GetDetailProfile();
        
        void StartProfileFrame();
        
        bool GetEndedThisFrame(std::string zone);
        double GetTime(std::string zone);
    }
}