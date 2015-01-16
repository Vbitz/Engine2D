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
#include "Events.hpp"

#ifdef _PLATFORM_WIN32
#include <Windows.h>
#undef CreateMutex
#endif

namespace Engine {
    namespace Logger {
        std::vector<LogEvent> _logEvents;
        
        bool _enableColor = false;
        
        std::ostringstream _ss;
        
        std::string _currentDomain;
        LogLevel _currentLogLevel;
        
        double _startTime = -1;
        
        Platform::MutexPtr _logMutex;
        
        bool _enableEvents = false;
        
        std::ostream& operator<<(std::ostream& os, const StreamFlusher& rhs) {
			LogText(_currentDomain, _currentLogLevel, _ss.str());
            _ss.str("");
            _currentDomain = "";
            _currentLogLevel = LogLevel_Error;
            return os.flush();
        }
        
        std::string getEscapeCode(int color, bool bold, bool bg) {
            return ("\x1b[" + std::string(bold ? "1;" : "0;") + std::to_string(color + 30) + "m");
        }
        
        void SetConsoleColor(bool reset, LogLevel level) {
#ifndef _PLATFORM_WIN32
			if (reset) {
				printf("%s", "\x1b[0;37m");
                return;
			}
            switch (level) {
                case LogLevel_Verbose:      printf("%s", getEscapeCode(0, true, false).c_str()); break;
				case LogLevel_User:         printf("%s", getEscapeCode(5, true, false).c_str()); break;
				case LogLevel_ConsoleInput: printf("%s", getEscapeCode(6, true, false).c_str()); break;
				case LogLevel_Log:          printf("%s", getEscapeCode(7, false, false).c_str()); break;
				case LogLevel_Warning:      printf("%s", getEscapeCode(3, false, false).c_str()); break;
				case LogLevel_Error:        printf("%s", getEscapeCode(1, true, false).c_str()); break;
                case LogLevel_Highlight:
				case LogLevel_Toast:        printf("%s", getEscapeCode(7, false, true).c_str()); printf("%s", getEscapeCode(0, true, false).c_str()); break;
				case LogLevel_TestLog:      printf("%s", getEscapeCode(7, false, true).c_str()); printf("%s", getEscapeCode(0, true, false).c_str()); break;
				case LogLevel_TestError:    printf("%s", getEscapeCode(1, false, true).c_str()); printf("%s", getEscapeCode(7, true, false).c_str()); break;
				default:					break;
            }
#else
			HANDLE hConsole = ::GetStdHandle(STD_OUTPUT_HANDLE);
			WORD colorCode = 0;

			if (reset) {
				colorCode = 7;
			} else {
				switch (level) {
				case LogLevel_Verbose:      colorCode = 0 + 8; break;
				case LogLevel_User:         colorCode = 5 + 8; break;
				case LogLevel_ConsoleInput: colorCode = 3 + 8; break;
				case LogLevel_Log:          colorCode = 7; break;
				case LogLevel_Warning:      colorCode = 6 + 8; break;
				case LogLevel_Error:        colorCode = 4 + 8; break;
				case LogLevel_Highlight:
				case LogLevel_Toast:        colorCode = (16 * 7) + 0; break;
				case LogLevel_TestLog:      colorCode = (16 * 7) + 0; break;
				case LogLevel_TestError:    colorCode = (16 * 1) + 4 + 8; break;
				default:					break;
				}
			}

			if (!::SetConsoleTextAttribute(hConsole, colorCode)) {
				std::cerr << "SetConsoleTextAttribute failed with: " << ::GetLastError() << std::endl;
			}
#endif
        }
        
        void Init() {
            _logMutex = Platform::CreateMutex();
        }
        
        void EnableLoggerEvents() {
            _enableEvents = true;
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
                case LogLevel_Highlight:
                    return "Highlight";
                case LogLevel_Toast:
                    return "Toast";
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
        :   Domain(domain), Level(level), Type(LogType_Text), Event(event), Hidden(false) {
            this->time = Platform::GetTime();
        }
        
        std::string LogEvent::FormatConsole() {
            std::stringstream ss;
            ss << "| " << this->time << " : [" << GetLevelString(this->Level) << "] " << this->Domain << " | " << this->Event;
            return ss.str();
        }
        
        void LogText(std::string domain, LogLevel level, std::string str) {
            bool logConsole = Config::GetBoolean("core.log.enableConsole")
                && (level != LogLevel_Verbose || Config::GetBoolean("core.log.levels.verbose"));
            if (logConsole && Config::GetBoolean("core.log.levels.onlyHighlight")) {
                if (level == LogLevel_Highlight || level == LogLevel_Toast ||
                    level == LogLevel_TestError || level == LogLevel_TestLog) {
                    logConsole = true;
                } else {
                    logConsole = false;
                }
            }

            bool showColors = Config::GetBoolean("core.log.showColors");
            
            _logMutex->Enter();
            
            if (_enableEvents) {
                Json::Value args(Json::objectValue);
                args["domain"] = domain;
                args["level"] = GetLevelString(level);
                args["str"] = str;
                args["isToast"] = (level == LogLevel_Toast);
                GetEventsSingilton()->GetEvent("logEvent")->Emit(args);
            }
            
            if (str.find('\n') == -1) {
                std::string newStr = cleanString(str);
                
                LogEvent newEvent(domain, level, newStr);
                
                try {
                    _logEvents.push_back(newEvent);
                } catch (std::exception e) {
                    std::cout << "Could not log: " << e.what() << std::endl;
                }

                if (logConsole) {
					if (showColors) {
						SetConsoleColor(false, level);
					}
					printf("%s", newEvent.FormatConsole().c_str());
					if (showColors) {
						SetConsoleColor(true, level);
					}
                    printf("\n");
                }
            } else {
                std::string strCopy = str;
                std::size_t newLinePos = strCopy.find('\n');
                
                while (newLinePos != std::string::npos) {
                    std::string newStr = cleanString(strCopy.substr(0, newLinePos));
                    
                    LogEvent newEvent(domain, level, newStr);
                    
                    _logEvents.push_back(newEvent);
                    
                    if (logConsole) {
						if (showColors) {
							SetConsoleColor(false, level);
						}
                        printf("%s", newEvent.FormatConsole().c_str());
                        if (showColors) {
                            SetConsoleColor(true, level);
                        }
                        printf("\n");
                    }
                    
                    strCopy.erase(0, newLinePos + 1);
                    newLinePos = strCopy.find('\n');
                }
                
				LogEvent newEvent(domain, level, strCopy);

				_logEvents.push_back(newEvent);
                
                if (logConsole) {
					if (showColors) {
						SetConsoleColor(false, level);
					}
                    printf("%s", newEvent.FormatConsole().c_str());
                    if (showColors) {
                        SetConsoleColor(true, level);
                    }
                    printf("\n");
                }
            }
            
            _logMutex->Exit();
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
        
        std::string DumpAllEvents() {
            std::stringstream ss;
            for (auto iter = _logEvents.begin(); iter != _logEvents.end(); iter++) {
                ss << iter->FormatConsole() << std::endl;
            }
            return ss.str();
        }
        
        void HideAllEvents() {
            for (auto iterator = _logEvents.begin(); iterator != _logEvents.end(); iterator++) {
                iterator->Hidden = true;
            }
        }
    }
}