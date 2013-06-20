#include "Profiler.hpp"

namespace Engine {
    namespace Profiler {
        
        struct ProfileEvent {
            double time;
            bool ended;
        };
        
        std::map<std::string, ProfileEvent> _zones;

        void Begin(std::string zone) {
            ProfileEvent newEvent;
            newEvent.time = Logger::GetTime();
            newEvent.ended = false;
            _zones[zone] = newEvent;
        }
        
        void End(std::string zone) {
            _zones[zone].time = (Logger::GetTime() - _zones[zone].time);
            _zones[zone].ended = true;
        }
        
        void ProfileZone(std::string zone, std::function<void()> func) {
            Begin(zone);
            func();
            End(zone);
        }
        
        void DumpProfile() {
            for (auto iter = _zones.begin(); iter != _zones.end(); iter++) {
                if (iter->second.ended) {
                    Logger::begin("Profiler", Logger::LogLevel_Verbose) << "ProfileEvent : " <<  iter->first << " : " << iter->second.time << Logger::end();
                }
            }
        }
    }
}