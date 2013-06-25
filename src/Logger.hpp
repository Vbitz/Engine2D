#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <sstream>

#ifdef _WIN32
	#include <sys\timeb.h>
#else
    #include <sys/time.h>
#endif

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
            LogLevel_Verbose,
            LogLevel_ConsoleInput
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