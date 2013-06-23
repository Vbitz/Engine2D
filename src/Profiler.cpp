#include "Profiler.hpp"

namespace Engine {
    namespace Profiler {
        
        struct ProfileEvent {
            std::string prettyName;
            double time;
            double lastTime;
            bool ended;
        };
        
        std::map<std::string, ProfileEvent> _zones;

        std::vector<std::string> _names;
        
        std::string _getPrettyName(std::string zoneName) {
            return _names[_names.size() - 1] + "::" + zoneName;
        }
        
        void Begin(std::string zone) {
            if (_names.size() == 0) {
                _names.push_back("Root");
            }
            if (_zones.count(zone) > 0) {
                _zones[zone].time = Logger::GetTime();
                _zones[zone].ended = false;
            } else {
                ProfileEvent newEvent;
                newEvent.prettyName = _getPrettyName(zone);
                newEvent.time = Logger::GetTime();
                newEvent.lastTime = 0;
                newEvent.ended = false;
                _zones[zone] = newEvent;
            }
            _names.push_back(_getPrettyName(zone));
        }
        
        void End(std::string zone) {
            _zones[zone].time = (Logger::GetTime() - _zones[zone].time);
            _zones[zone].lastTime = _zones[zone].time;
            _zones[zone].ended = true;
            _names.pop_back();
        }
        
        void ProfileZone(std::string zone, std::function<void()> func) {
            Begin(zone);
            func();
            End(zone);
        }
        
        void DumpProfile() {
            for (auto iter = _zones.begin(); iter != _zones.end(); iter++) {
                if (iter->second.ended) {
                    Logger::begin("Profiler", Logger::LogLevel_Verbose) << "ProfileEvent : " <<  iter->second.prettyName << " : " << iter->second.lastTime << "s" << Logger::end();
                }
            }
        }
        
        double GetTime(std::string zone) {
            return _zones[zone].lastTime;
        }
    }
}