/*
   Filename: Timer.cpp
   Purpose:  Timing subsystem for tracking tied to event module

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

#include "Timer.hpp"
#include "Events.hpp"

#include "Platform.hpp"
#include "Logger.hpp"

#include <map>

namespace Engine {
    namespace Timer {
        unsigned int _lastTimer = 0;
        double _pauseTime = 0;
        
        struct TimerStatus {
            std::string eventTarget;
            double interval;
            double targetTime;
            bool repeat;
        };
        
        std::map<unsigned int, TimerStatus> _timers;
        
        void Update() {
            std::vector<unsigned int> deleteTargets;
            double currentTime = Platform::GetTime() - _pauseTime;
            for (auto iter = _timers.begin(); iter != _timers.end(); iter++) {
                if (currentTime > iter->second.targetTime) {
                    Events::Emit(iter->second.eventTarget);
                    if (iter->second.repeat) {
                        iter->second.targetTime += iter->second.interval;
                    } else {
                        deleteTargets.push_back(iter->first);
                    }
                }
            }
            for (auto iter = deleteTargets.begin(); iter != deleteTargets.end(); iter++) {
                _timers.erase(*iter);
            }
        }
        
        int Create(double time, std::string event) {
            return Create(time, event, false);
        }
        
        int Create(double time, std::string event, bool repeat) {
            TimerStatus s;
            s.interval = time;
            s.targetTime = Platform::GetTime() + s.interval;
            s.eventTarget = event;
            s.repeat = repeat;
            _timers[++_lastTimer] = s;
            return _lastTimer;
        }
        
        void Remove(int timer) {
            _timers.erase(timer);
        }
        
        void NotifyPause(double time) {
            Logger::begin("Timer", Logger::LogLevel_Verbose) << "Detected pause of: " << time << "s" << Logger::end();
            _pauseTime += time;
        }
    }
}