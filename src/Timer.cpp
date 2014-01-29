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
            double currentTime = Platform::GetTime() - _pauseTime;
            for (auto iter = _timers.begin(); iter != _timers.end(); iter++) {
                if (currentTime > iter->second.targetTime) {
                    Events::Emit(iter->second.eventTarget);
                    if (iter->second.repeat) {
                        iter->second.targetTime += iter->second.interval;
                    } else {
                        iter = _timers.erase(iter);
                        iter--;
                    }
                }
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