#pragma once

#include "common.hpp"

#include <vector>
#include <sstream>

namespace Engine {
    namespace Logger {
        class StreamFlusher { };
        std::ostream& operator<<(std::ostream& os, const StreamFlusher& rhs);
        
        double GetTime();
        
        enum LogLevel {
            LogLevel_Error,
            LogLevel_Warning,
            LogLevel_Log,
            LogLevel_User,
            LogLevel_Verbose
        };
        
        class LogEvent {
        public:
            LogEvent(std::string domain, LogLevel level, std::string event)
            :   Domain(domain), Level(level), Event(event) {
                this->time = GetTime();
            }
            
            std::string Domain;
            LogLevel Level;
            std::string Event;
            double time;
        };
        
        std::string GetLevelString(LogLevel level);
        
        void LogText(std::string domain, LogLevel level, std::string str);
        
        std::ostream& begin(std::string domain, LogLevel level);
        StreamFlusher end();
        
        std::vector<LogEvent>* GetEvents();
    }
}