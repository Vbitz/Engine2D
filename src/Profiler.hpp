/*
   Filename: Profiler.hpp
   Purpose:  Code performance profiler

   Part of Engine2D

   Copyright (C) 2014 Vbitz

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

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