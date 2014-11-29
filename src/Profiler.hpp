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
#include <sstream>

#include "stdlib.hpp"
#include "Platform.hpp"
#include "vendor/json/json.h"

#ifdef PROFILER
#define ENGINE_PROFILER_SCOPE Engine::Profiler::Scope __PROFILER_SCOPE__(__PRETTY_FUNCTION__)
#define ENGINE_PROFILER_SCOPE_EX(str) Engine::Profiler::Scope __PROFILER_SCOPE__(__PRETTY_FUNCTION__, str)
#else
#define ENGINE_PROFILER_SCOPE
#define ENGINE_PROFILER_SCOPE_EX
#endif

namespace Engine {
    namespace Profiler {
        struct ProfileZoneMetadata {
            ProfileZoneMetadata *parent = NULL;
            std::string name;
            unsigned int callCount = 0;
            double totalTime = 0;
            double totalSelfTime = 0;
            double avgTime = -1; // Using a CMA (Cumulative Moving Average)
            double minTime = std::numeric_limits<double>::max();
            double maxTime = std::numeric_limits<double>::min();
            std::unordered_map<std::string, ProfileZoneMetadata*> children;
        };
        
        typedef ProfileZoneMetadata* ProfileZoneMetadataPtr;
        
        ENGINE_CLASS(Scope);
        
        void BeginProfile(ScopePtr scope);
        void SubmitProfile(ScopePtr scope);
        
        class Scope {
        public:
            Scope(const char* functionName) {
                this->_name = functionName;
                BeginProfile(this);
                this->_startTime = Platform::GetTime();
            }
            
            Scope(const char* functionName, const char* exName) {
                std::stringstream ss;
                ss << functionName << " : " << exName;
                this->_name = ss.str().c_str();
                BeginProfile(this);
                this->_startTime = Platform::GetTime();
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
        
        Json::Value GetLastFrame();
    }
}