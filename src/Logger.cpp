/*
   Filename: Logger.cpp
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

#include "Logger.hpp"

#include "Config.hpp"
#include "Platform.hpp"

namespace Engine {
    namespace Logger {
        std::vector<LogEvent> _logEvents;
        
        bool _enableColor = false;
        
        std::ostringstream _ss;
        
        std::string _currentDomain;
        LogLevel _currentLogLevel;
        
        double _startTime = -1;
        
        std::ostream& operator<<(std::ostream& os, const StreamFlusher& rhs) {
			LogText(_currentDomain, _currentLogLevel, _ss.str());
            _ss.str("");
            _currentDomain = "";
            _currentLogLevel = LogLevel_Error;
            return os.flush();
        }
        
        std::string getEscapeCode(int color, bool bold) {
            return "\x1b[" + std::string(bold ? "1;" : "0;") + std::to_string(color + 30) + "m| ";
        }
        
        std::string GetLevelColor(LogLevel level) {
#ifndef _PLATFORM_WIN32
            switch (level) {
                case LogLevel_Verbose:      return getEscapeCode(0, true);
                case LogLevel_User:         return getEscapeCode(5, true);
                case LogLevel_ConsoleInput: return getEscapeCode(6, true);
                case LogLevel_Log:          return getEscapeCode(7, false);
                case LogLevel_Warning:      return getEscapeCode(3, true);
                case LogLevel_Error:        return getEscapeCode(1, true);
                case LogLevel_TestLog:      return "\x1b[0;47m\x1b[1;30m| ";
                case LogLevel_TestError:    return "\x1b[0;41m\x1b[1;37m| ";
				default:					return "";
            }
#else
			return ""; // windows is not a very sane OS
#endif
        }
        
        std::string GetLevelString(LogLevel level) {
            switch (level) {
                case LogLevel_ConsoleInput:
                    return "ConsoleInput";
                case LogLevel_Verbose:
                    return "Verbose";
                case LogLevel_User:
                    return "User";
                case LogLevel_Log:
                    return "Log";
                case LogLevel_Warning:
                    return "Warning";
                case LogLevel_Error:
                    return "Error";
                case LogLevel_TestLog:
                    return "TestLog";
                case LogLevel_TestError:
                    return "TestError";
				default:
					return "Unknown";
            }
        }
        
        std::string cleanString(std::string str) {
            std::size_t lastPos = 0;
            while (str.find('\t', lastPos) != std::string::npos) {
                std::size_t pos = str.find('\t', lastPos);
                str.replace(pos, 1, "    "); // replace tabs with 4 spaces
                lastPos = pos + 1;
            }
            return str;
        }
        
        LogEvent::LogEvent(std::string domain, LogLevel level, std::string event)
        :   Domain(domain), Level(level), Type(LogType_Text),
        GraphEvent(NULL), Event(event), Hidden(false) {
            this->time = Platform::GetTime();
        }
        
        LogEvent::LogEvent(std::string domain, LogLevel level, LogGraphEvent* event)
        :   Domain(domain), Level(level), Type(LogType_Graphical),
        GraphEvent(event), Event(""), Hidden(false) {
            this->time = Platform::GetTime();
        }
        
        void LogText(std::string domain, LogLevel level, std::string str) {
            bool logConsole = Config::GetBoolean("core.log.enableConsole")
                && (level != LogLevel_Verbose || Config::GetBoolean("core.log.levels.verbose"));
            std::string colorCode = Config::GetBoolean("core.log.showColors") ? GetLevelColor(level) : "";
            
            if (str.find('\n') == -1) {
                std::string newStr = cleanString(str);
                
                try {
                    _logEvents.push_back(LogEvent(domain, level, newStr));
                } catch (std::exception e) {
                    std::cout << "Could not log: " << e.what() << std::endl;
                }

                if (logConsole) {
                    std::cout << colorCode << Platform::GetTime()
                    << " : [" << GetLevelString(level) << "] "
                    << domain << " | " << newStr << "\x1b[0;37m" << std::endl;
                }
            } else {
                std::string strCopy = str;
                std::size_t newLinePos = strCopy.find('\n');
                
                while (newLinePos != std::string::npos) {
                    std::string newStr = cleanString(strCopy.substr(0, newLinePos));

                    _logEvents.push_back(LogEvent(domain, level, newStr));
                    
                    if (logConsole) {
                        std::cout << colorCode << Platform::GetTime()
                            << " : [" << GetLevelString(level) << "] "
                            << domain << " | " << newStr << "\x1b[0;37m" << std::endl;
                    }
                    
                    strCopy.erase(0, newLinePos + 1);
                    newLinePos = strCopy.find('\n');
                }
                
                _logEvents.push_back(LogEvent(domain, level, strCopy));
                
                if (logConsole) {
                    std::cout << colorCode << Platform::GetTime()
                        << " : [" << GetLevelString(level) << "] "
                        << domain << " | " << strCopy << std::endl;
                }
            }
        }
        
        void LogGraph(std::string domain, LogLevel level, LogGraphEvent* event) {
            bool logConsole = Config::GetBoolean("core.log.enableConsole")
            && (level != LogLevel_Verbose || Config::GetBoolean("core.log.levels.verbose"));
            std::string colorCode = Config::GetBoolean("core.log.showColors") ? GetLevelColor(level) : "";
            
            _logEvents.push_back(LogEvent(domain, level, event));
            
            if (logConsole) {
                std::cout << colorCode << Platform::GetTime()
                << " : [" << GetLevelString(level) << "] "
                << domain << " | Graphical" << std::endl;
            }
        }
        
        std::ostream& begin(std::string domain, LogLevel level) {
            _ss.str("");
            _currentDomain = domain;
            _currentLogLevel = level;
			return _ss;
        }
        
        StreamFlusher end() {
            return StreamFlusher();
        }
        
        std::vector<LogEvent>* GetEvents() {
            return &_logEvents;
        }
        
        void HideAllEvents() {
            for (auto iterator = _logEvents.begin(); iterator != _logEvents.end(); iterator++) {
                iterator->Hidden = true;
            }
        }
    }
}