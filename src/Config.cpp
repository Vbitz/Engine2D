#include "Config.hpp"

namespace Engine {
    namespace Config {
        std::map<std::string, float> _numberCvars;
        std::map<std::string, bool> _boolCvars;
        std::map<std::string, std::string> _stringCvars;
        
        void SetNumber(std::string key, int value) {
            _numberCvars[key] = value;
        }
        
        void SetNumber(std::string key, float value) {
            _numberCvars[key] = value;
        }
        
        void SetBoolean(std::string key, bool value) {
            _boolCvars[key] = value;
        }
        
        void SetString(std::string key, std::string value) {
            _stringCvars[key] = value;
        }
        
        
        std::string Get(std::string key) {
            for (auto iter = _numberCvars.begin(); iter != _numberCvars.end(); iter++) {
                if (iter->first == key) {
                    return std::to_string(iter->second);
                }
            }
            for (auto iter = _boolCvars.begin(); iter != _boolCvars.end(); iter++) {
                if (iter->first == key) {
                    return std::to_string(iter->second);
                }
            }
            for (auto iter = _stringCvars.begin(); iter != _stringCvars.end(); iter++) {
                if (iter->first == key) {
                    return iter->second;
                }
            }
            
            return "undefined";
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
        
        bool HasConfig(std::string key) {
            for (auto iter = _numberCvars.begin(); iter != _numberCvars.end(); iter++) {
                if (iter->first == key) {
                    return true;
                }
            }
            for (auto iter = _boolCvars.begin(); iter != _boolCvars.end(); iter++) {
                if (iter->first == key) {
                    return true;
                }
            }
            for (auto iter = _stringCvars.begin(); iter != _stringCvars.end(); iter++) {
                if (iter->first == key) {
                    return true;
                }
            }
            
            return true;
        }
        
        std::vector<std::string> GetAll() {
            std::vector<std::string> ret;
            for (auto iter = _numberCvars.begin(); iter != _numberCvars.end(); iter++) {
                ret.push_back("number : " + iter->first + " : " + std::to_string(iter->second));
            }
            for (auto iter = _boolCvars.begin(); iter != _boolCvars.end(); iter++) {
                ret.push_back("bool : " + iter->first + " : " + (iter->second ? "true" : "false"));
            }
            for (auto iter = _stringCvars.begin(); iter != _stringCvars.end(); iter++) {
                ret.push_back("string : " + iter->first + " : " + iter->second);
            }
            return ret;
        }
    }
}