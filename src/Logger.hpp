/*
   Filename: Logger.hpp
   Purpose:  Logging subsystem

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
            LogLevel_Highlight,
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
            
            std::string FormatConsole();
            
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
        
        std::string DumpAllEvents();
        
        void HideAllEvents();
        
        std::vector<LogEvent>* GetEvents();
    }
}