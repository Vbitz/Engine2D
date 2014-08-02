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

#include "stdlib.hpp"
#include "Platform.hpp"

#define PROFILER

#ifdef PROFILER
#define ENGINE_PROFILER_SCOPE Engine::Profiler_New::Scope __PROFILER_SCOPE__(__PRETTY_FUNCTION__)
#else
#define ENGINE_PROFILER_SCOPE
#endif

namespace Engine {
    namespace Profiler_New {
        ENGINE_CLASS(Scope);
        
        void BeginProfile(ScopePtr scope);
        void SubmitProfile(ScopePtr scope);
        
        class Scope {
        public:
            Scope(const char* functionName) {
                this->_startTime = Platform::GetTime();
                this->_name = functionName;
                BeginProfile(this);
            }
            
            void Close() {
                if (this->_running) {
                    this->_endTime = Platform::GetTime();
                    SubmitProfile(this);
                    this->_running = false;
                }
            }
            
            ~Scope() {
                this->Close();
            }
            
            std::string GetName() {
                return std::string(this->_name);
            }
            
            double GetElapsedTime() {
                return this->_endTime - this->_startTime;
            }
        private:
            const char* _name;
            double _startTime;
            double _endTime;
            bool _running = true;
        };
        
        void BeginProfileFrame();
        void EndProfileFrame();
    }
    
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