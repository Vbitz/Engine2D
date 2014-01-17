#pragma once

#include <vector>
#include <string>

namespace Engine {
    namespace Profiler {
        void Begin(std::string zone);
        void Begin(std::string zone, double maxTime);
        void End(std::string zone);
        
        void DumpProfile();
        
        void ResetDetail();
        void CaptureDetail();
        std::string GetDetailProfile();
        
        void StartProfileFrame();
        
        bool GetEndedThisFrame(std::string zone);
        double GetTime(std::string zone);
        
        std::vector<std::string> GetZones();
        std::vector<std::string> GetZones(bool prettyName);
        
        void SetMaxTime(std::string zone, double time);
        void EnableMaxTime(std::string zone, double time);
        void DisableMaxTime(std::string zone);
    }
}