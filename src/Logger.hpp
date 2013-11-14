#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <functional>

namespace Engine {
    namespace Logger {
        class StreamFlusher { };
        std::ostream& operator<<(std::ostream& os, const StreamFlusher& rhs);
        
        enum LogLevel {
            LogLevel_Error,
            LogLevel_Warning,
            LogLevel_Log,
            LogLevel_User,
            LogLevel_Verbose,
            LogLevel_ConsoleInput,
            LogLevel_TestLog,
            LogLevel_TestError
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
            LogEvent(std::string domain, LogLevel level, std::string event);
            LogEvent(std::string domain, LogLevel level, LogGraphEvent* event);
            
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