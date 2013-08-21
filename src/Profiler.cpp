#include "Profiler.hpp"

namespace Engine {
    namespace Profiler {
        
        std::stringstream _detail;
        std::vector<std::string> _detailCapturePoints;
        
        struct ProfileEvent {
            std::string prettyName;
            double time;
            double lastTime;
            double min;
            double max;
            bool ended;
            bool thisFrame;
            double maxTime = 0;
            bool reportLong = false;
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
            if (!Config::GetBoolean("cl_profiler")) {
                return;
            }
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
            if (!Config::GetBoolean("cl_profiler")) {
                return;
            }
            ProfileEvent& e = _zones[zone];
            e.time = (Logger::GetTime() - e.time);
            e.lastTime = e.time;
            e.ended = true;
            e.max = _max(e.max, e.lastTime);
            e.min = _min(e.min, e.lastTime);
            e.thisFrame = true;
            if (e.reportLong && e.time > e.maxTime && Config::GetBoolean("log_profiler_maxTime")) {
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
            _detail << Logger::GetTime();
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
            if (!Config::GetBoolean("cl_profiler")) {
                return;
            }
            
            for (auto iter = _zones.begin(); iter != _zones.end(); iter++) {
                iter->second.thisFrame = false;
            }
        }
        
        bool GetEndedThisFrame(std::string zone) {
            return _zones[zone].thisFrame && _zones[zone].ended;
        }
        
        double GetTime(std::string zone) {
            return _zones[zone].lastTime;
        }
        
        std::vector<std::string> GetZones() {
            std::vector<std::string> ret;
            for (auto iter = _zones.begin(); iter != _zones.end(); iter++) {
                ret.push_back(iter->second.prettyName);
            }
            return ret;
        }
        
        void SetMaxTime(std::string zone, double time) {
            _zones[zone].maxTime = time;
        }
        
        void EnableMaxTime(std::string zone, double time) {
            _zones[zone].reportLong = true;
            _zones[zone].maxTime = time;
        }
        
        void DisableMaxTime(std::string zone) {
            _zones[zone].reportLong = false;
        }
    }
}