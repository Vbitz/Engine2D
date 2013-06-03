#include "Logger.hpp"

namespace Engine {
    namespace Logger {
        std::vector<LogEvent> _logEvents;
        
        std::ostringstream _ss;
        
        std::string _currentDomain;
        LogLevel _currentLogLevel;
        
        double _startTime = -1;
        
        std::ostream& operator<<(std::ostream& os, const StreamFlusher& rhs) {
            return os.flush();
        }
        
        std::string GetLevelString(LogLevel level) {
            switch (level) {
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
            _logEvents.push_back(LogEvent(domain, level, str));
            std::cout << GetTime() << " : [" << GetLevelString(level) << "] " << domain << " | " << str << std::endl;
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