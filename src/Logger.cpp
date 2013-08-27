#include "Logger.hpp"

#include "Config.hpp"

namespace Engine {
    namespace Logger {
        std::vector<LogEvent> _logEvents;
        
        bool _enableColor = false;
        
        std::ostringstream _ss;
        
        std::string _currentDomain;
        LogLevel _currentLogLevel;
        
        double _startTime = -1;
        
        std::ostream& operator<<(std::ostream& os, const StreamFlusher& rhs) {
            return os.flush();
        }
        
        std::string getEscapeCode(int color, bool bold) {
            return "\x1b[" + std::to_string(color + 30) + (bold ? ";1m| " : "m| ");
        }
        
        std::string GetLevelColor(LogLevel level) {
            switch (level) {
                case LogLevel_Verbose:      return getEscapeCode(0, true);
                case LogLevel_User:         return getEscapeCode(5, true);
                case LogLevel_ConsoleInput: return getEscapeCode(6, true);
                case LogLevel_Log:          return getEscapeCode(7, false);
                case LogLevel_Warning:      return getEscapeCode(3, true);
                case LogLevel_Error:        return getEscapeCode(1, true);
            }
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
            }
        }
        
        double GetTime() {
#ifdef _PLATFORM_WIN32
            double time = GetTickCount() / 1000; // it's really bad compared to unix but it will do.
#else
            timeval _time;
            gettimeofday(&_time, NULL);
            
            double time = _time.tv_sec + (_time.tv_usec * 0.000001);
#endif
            if (_startTime < 0) {
                _startTime = time;
                return 0;
            } else {
                return time - _startTime;
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
        
        void LogText(std::string domain, LogLevel level, std::string str) {
            bool logConsole = Config::GetBoolean("log_console")
                && (level != LogLevel_Verbose || Config::GetBoolean("log_consoleVerbose"));
            std::string colorCode = Config::GetBoolean("log_colors") ? GetLevelColor(level) : "";
            
            if (str.find('\n') == -1) {
                std::string newStr = cleanString(str);
                
                _logEvents.push_back(LogEvent(domain, level, newStr));
                
                if (logConsole) {
                    std::cout << colorCode << GetTime()
                    << " : [" << GetLevelString(level) << "] "
                    << domain << " | " << newStr << std::endl;
                }
            } else {
                std::string strCopy = str;
                std::size_t newLinePos = strCopy.find('\n');
                
                while (newLinePos != std::string::npos) {
                    std::string newStr = cleanString(strCopy.substr(0, newLinePos));
                    
                    _logEvents.push_back(LogEvent(domain, level, newStr));
                    
                    if (logConsole) {
                        std::cout << colorCode << GetTime()
                            << " : [" << GetLevelString(level) << "] "
                            << domain << " | " << newStr << std::endl;
                    }
                    
                    strCopy.erase(0, newLinePos + 1);
                    newLinePos = strCopy.find('\n');
                }
                
                _logEvents.push_back(LogEvent(domain, level, strCopy));
                
                if (logConsole) {
                    std::cout << colorCode << GetTime()
                        << " : [" << GetLevelString(level) << "] "
                        << domain << " | " << strCopy << std::endl;
                }
            }
        }
        
        void LogGraph(std::string domain, LogLevel level, LogGraphEvent* event) {
            bool logConsole = Config::GetBoolean("log_console")
                && (level != LogLevel_Verbose || Config::GetBoolean("log_consoleVerbose"));
            std::string colorCode = Config::GetBoolean("log_colors") ? GetLevelColor(level) : "";
            
            _logEvents.push_back(LogEvent(domain, level, event));
            
            if (logConsole) {
                std::cout << colorCode << GetTime()
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
            LogText(_currentDomain, _currentLogLevel, _ss.str());
            _ss.str("");
            _currentDomain = "";
            _currentLogLevel = LogLevel_Error;
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