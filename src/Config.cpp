#include "Config.hpp"

namespace Engine {
    namespace Config {
        std::map<std::string, float> _numberCvars;
        std::map<std::string, bool> _boolCvars;
        std::map<std::string, std::string> _stringCvars;
        
        void Set(std::string key, int value) {
            _numberCvars[key] = value;
        }
        
        void Set(std::string key, float value) {
            _numberCvars[key] = value;
        }
        
        void Set(std::string key, bool value) {
            _boolCvars[key] = value;
        }
        
        void Set(std::string key, std::string value) {
            _stringCvars[key] = value;
        }
        
        
        int GetInt(std::string key) {
            return (int) _numberCvars[key];
        }
        
        float GetFloat(std::string key) {
            return _numberCvars[key];
        }
        
        bool GetBoolean(std::string key) {
            return _boolCvars[key];
        }
        
        std::string GetString(std::string key) {
            return _stringCvars[key];
        }
    }
}