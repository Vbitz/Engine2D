#include "Timer.hpp"
#include "Events.hpp"

#include "FramePerfMonitor.hpp"

#include <iostream>
#include <map>

namespace Engine {
    namespace Timer {
        unsigned int _lastTimer = 0;
        
        struct TimerStatus {
            std::string eventTarget;
            double currentTime;
            double targetTime;
            bool repeat;
        };
        
        std::map<unsigned int, TimerStatus> _timers;
        
        void Update() {
            double dt = FramePerfMonitor::GetFrameTime();
            for (auto iter = _timers.begin(); iter != _timers.end(); iter++) {
                iter->second.currentTime += dt;
                if (iter->second.currentTime > iter->second.targetTime) {
                    Events::Emit(iter->second.eventTarget);
                    if (iter->second.repeat) {
                        iter->second.currentTime = 0;
                    } else {
                        iter = _timers.erase(iter);
                        if (_timers.size() == 0) {
                            return;
                        }
                    }
                }
            }
        }
        
        int Create(double time, std::string event) {
            Create(time, event, false);
        }
        
        int Create(double time, std::string event, bool repeat) {
            TimerStatus s;
            s.currentTime = 0.0;
            s.targetTime = time;
            s.eventTarget = event;
            s.repeat = repeat;
            _timers[++_lastTimer] = s;
        }
        
        void Remove(int timer) {
            _timers.erase(timer);
        }
    }
}