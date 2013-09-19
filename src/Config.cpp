#include "Config.hpp"

#include <iostream>

namespace Engine {
    namespace Config {
        std::unordered_map<std::string, float> _numberCvars;
        std::unordered_map<std::string, bool> _boolCvars;
        std::unordered_map<std::string, std::string> _stringCvars;
        
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
            //std::cout << "Get Int: " << key << std::endl;
            return (int) _numberCvars[key];
        }
        
        float GetFloat(std::string key) {
            //std::cout << "Get Float: " << key << std::endl;
            return _numberCvars[key];
        }
        
        bool GetBoolean(std::string key) {
            //std::cout << "Get Bool: " << key << std::endl;
            return _boolCvars[key];
        }
        
        std::string GetString(std::string key) {
            //std::cout << "Get String: " << key << std::endl;
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