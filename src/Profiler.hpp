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
        struct ProfileZoneMetadata {
            ProfileZoneMetadata *parent = NULL;
            std::string name;
            unsigned int callCount = 0;
            double totalTime = 0;
            double avgTime = -1; // Using a CMA (Cumulative Moving Average)
            double minTime = std::numeric_limits<double>::max();
            double maxTime = std::numeric_limits<double>::min();
            std::unordered_map<std::string, ProfileZoneMetadata*> children;
        };
        
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
}