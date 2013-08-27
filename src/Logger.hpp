#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <functional>

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
        
        enum LogType {
            LogType_Text,
            LogType_Graphical
        };
        
        class LogGraphEvent {
        public:
            LogGraphEvent() {};
            ~LogGraphEvent() {};
            
            virtual int Run(int x, int y) {
                return 0;
            }
            
            int operator()(int x, int y) {
                return this->Run(x, y);
            }
        };
        
        class LogEvent {
        public:
            LogEvent(std::string domain, LogLevel level, std::string event)
            :   Domain(domain), Level(level), Type(LogType_Text),
                GraphEvent(NULL), Event(event), Hidden(false) {
                this->time = GetTime();
            }
            
            LogEvent(std::string domain, LogLevel level, LogGraphEvent* event)
            :   Domain(domain), Level(level), Type(LogType_Graphical),
                GraphEvent(event), Event(""), Hidden(false) {
                this->time = GetTime();
            }
            
            std::string Domain;
            LogLevel Level;
            LogType Type;
            LogGraphEvent* GraphEvent;
            std::string Event;
            bool Hidden;
            double time;
        };
        
        std::string GetLevelString(LogLevel level);
        
        void LogText(std::string domain, LogLevel level, std::string str);
        void LogGraph(std::string domain, LogLevel level, LogGraphEvent* event);
        
        std::ostream& begin(std::string domain, LogLevel level);
        StreamFlusher end();
        
        void HideAllEvents();
        
        std::vector<LogEvent>* GetEvents();
    }
}