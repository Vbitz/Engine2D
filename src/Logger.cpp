#include "Logger.hpp"

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
            double time = GetTickCount();
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
        
        void LogText(std::string domain, LogLevel level, std::string str) {
            if (str.find('\n') == -1) {
                _logEvents.push_back(LogEvent(domain, level, str));
                std::cout << GetLevelColor(level) << GetTime()
                    << " : [" << GetLevelString(level) << "] "
                    << domain << " | " << str << std::endl;
            } else {
                std::string strCopy = str;
                std::size_t newLinePos = strCopy.find('\n');
                while (newLinePos != std::string::npos) {
                    _logEvents.push_back(LogEvent(domain, level, strCopy.substr(0, newLinePos - 1)));
                    std::cout << GetLevelColor(level) << GetTime()
                        << " : [" << GetLevelString(level) << "] "
                        << domain << " | " << strCopy.substr(0, newLinePos) << std::endl;
                    strCopy.erase(0, newLinePos + 1);
                    newLinePos = strCopy.find('\n');
                }
                _logEvents.push_back(LogEvent(domain, level, strCopy));
                std::cout << GetLevelColor(level) << GetTime()
                    << " : [" << GetLevelString(level) << "] "
                    << domain << " | " << strCopy << std::endl;
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
    }
}