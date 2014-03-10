/*
   Filename: Profiler.cpp
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

#include "Profiler.hpp"

#include <functional>
#include <unordered_map>
#include <sstream>
#include <map>

#include "Logger.hpp"
#include "Config.hpp"

#include "Platform.hpp"

namespace Engine {
    namespace Profiler {
        
        std::stringstream _detail;
        std::vector<std::string> _detailCapturePoints;
        
        struct ProfileEvent {
            std::string name;
            std::string prettyName;
            double time;
            double lastTime;
            double min;
            double max;
            bool ended;
            bool thisFrame;
            double maxTime;
            bool reportLong;
        };
        
        std::unordered_map<std::string, ProfileEvent> _zones;

        std::vector<std::string> _names;
        
        std::string _getPrettyName(std::string zoneName) {
            return _names[_names.size() - 1] + "::" + zoneName;
        }
        
        inline double _min(double num1, double num2) {
            return num1 > num2 ? num2 : num1;
        }
        
        inline double _max(double num1, double num2) {
            return num1 > num2 ? num1 : num2;
        }
        
        void Begin(std::string zone) {
            Begin(zone, -1);
        }
        
        void Begin(std::string zone, double maxTime) {
            if (!Config::GetBoolean("core.debug.profiler")) {
                return;
            }
            if (_names.size() == 0) {
                _names.push_back("Root");
            }
            if (_zones.count(zone) > 0) {
                _zones[zone].time = Platform::GetTime();
                _zones[zone].ended = false;
            } else {
                //Logger::begin("ProfilerDebug", Logger::LogLevel_Verbose) << "Register: " << zone << Logger::end();
                ProfileEvent newEvent;
                newEvent.name = zone;
                newEvent.prettyName = _getPrettyName(zone);
                newEvent.time = Platform::GetTime();
                newEvent.min = 99999999999;
                newEvent.lastTime = newEvent.max = 0;
                newEvent.ended = newEvent.thisFrame = false;
                newEvent.maxTime = maxTime;
                newEvent.reportLong = maxTime > -1;
                _zones[zone] = newEvent;
            }
            _names.push_back(_getPrettyName(zone));
        }
        
        void End(std::string zone) {
            if (!Config::GetBoolean("core.debug.profiler")) {
                return;
            }
            if (_zones.count(zone) == 0) {
                return;
            }
            ProfileEvent& e = _zones[zone];
            e.time = (Platform::GetTime() - e.time);
            e.lastTime = e.time;
            e.ended = true;
            e.max = _max(e.max, e.lastTime);
            e.min = _min(e.min, e.lastTime);
            e.thisFrame = true;
            if (e.reportLong && e.time > e.maxTime && Config::GetBoolean("core.log.src.perfIssues")) {
                Logger::begin("Profiler", Logger::LogLevel_Warning)
                    << "ProfileEvent: " << e.prettyName << " : "
                    << "Exceeded Max Time: " << e.maxTime << " : "
                    << "By: " << ((e.time / e.maxTime * 100) - 100) << "%"
                    << Logger::end();
            }
            _names.pop_back();
        }
        
        void DumpProfile() {
            for (auto iter = _zones.begin(); iter != _zones.end(); iter++) {
                Logger::begin("Profiler", Logger::LogLevel_Verbose)
                    << "ProfileEvent : " << iter->second.prettyName
                    << " : last: " << iter->second.lastTime << "s"
                    << " : min: " << iter->second.min << "s"
                    << " : max: " << iter->second.max << "s"
                    << Logger::end();
            }
        }
        
        
        void ResetDetail() {
            _detail.str("");
            _detailCapturePoints.clear();
            _detail << "time";
            for (auto iter = _zones.begin(); iter != _zones.end(); iter++) {
                _detailCapturePoints.push_back(iter->first);
                _detail << ',' << iter->second.prettyName;
            }
        }
        
        void CaptureDetail() {
            _detail << std::endl;
            _detail << Platform::GetTime();
            for (auto iter = _detailCapturePoints.begin(); iter != _detailCapturePoints.end(); iter++) {
                _detail << ',';
                if (GetEndedThisFrame(*iter)) {
                    _detail << GetTime(*iter);
                }
            }
        }
        
        std::string GetDetailProfile() {
            return _detail.str();
        }
        
        void StartProfileFrame() {
            if (!Config::GetBoolean("core.debug.profiler")) {
                return;
            }
            
            for (auto iter = _zones.begin(); iter != _zones.end(); iter++) {
                iter->second.thisFrame = false;
            }
        }
        
        bool GetEndedThisFrame(std::string zone) {
            if (_zones.count(zone) > 0) {
                return _zones[zone].thisFrame && _zones[zone].ended;
            } else {
                return false;
            }
        }
        
        double GetTime(std::string zone) {
            if (_zones.count(zone) > 0) {   // yay for C++
                    // If you call one of these functions on a zone not yet created
                    // it will create it for you in a semi corupted state, it will be fixed when it ends though
                return _zones[zone].lastTime;
            } else {
                return -1;
            }
        }
        
        std::vector<std::string> GetZones() {
            return GetZones(false);
        }
        
        std::vector<std::string> GetZones(bool pretty) {
            std::vector<std::string> ret;
            for (auto iter = _zones.begin(); iter != _zones.end(); iter++) {
                ret.push_back(pretty ? iter->second.prettyName : iter->first);
            }
            return ret;
        }
        
        void SetMaxTime(std::string zone, double time) {
            if (_zones.count(zone) > 0) {
                _zones[zone].maxTime = time;
            }
        }
        
        void EnableMaxTime(std::string zone, double time) {
            if (_zones.count(zone) > 0) {
                _zones[zone].reportLong = true;
                _zones[zone].maxTime = time;
            }
        }
        
        void DisableMaxTime(std::string zone) {
            if (_zones.count(zone) > 0) {
                _zones[zone].reportLong = false;
            }
        }
    }
}