/*
   Filename: Config.cpp
   Purpose:  Common runtime configuration storage

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

#include "Config.hpp"

#include "Events.hpp"

namespace Engine {
    namespace Config {
        std::unordered_map<std::string, float> _numberCvars;
        std::unordered_map<std::string, bool> _boolCvars;
        std::unordered_map<std::string, std::string> _stringCvars;
        
        bool _configEventEnabled = false;
        
        void EnableConfigEvents() {
            _configEventEnabled = true;
        }
        
        void SetNumber(std::string key, int value) {
            _numberCvars[key] = value;
            if (_configEventEnabled) Events::GetEvent("config:" + key)->Emit();
        }
        
        void SetNumber(std::string key, float value) {
            _numberCvars[key] = value;
            if (_configEventEnabled) Events::GetEvent("config:" + key)->Emit();
        }
        
        void SetBoolean(std::string key, bool value) {
            _boolCvars[key] = value;
            if (_configEventEnabled) Events::GetEvent("config:" + key)->Emit();
        }
        
        void SetString(std::string key, std::string value) {
            _stringCvars[key] = value;
            if (_configEventEnabled) Events::GetEvent("config:" + key)->Emit();
        }
        
        bool Set(std::string key, std::string value) {
            switch (GetType(key)) {
                case ConfigType_String:
                    SetString(key, value);
                    break;
                case ConfigType_Number:
                    SetNumber(key, std::stof(value));
                    break;
                case ConfigType_Bool:
                    if (value == "true") {
                        SetBoolean(key, true);
                    } else if (value == "false") {
                        SetBoolean(key, false);
                    } else {
                        return false;
                    }
                    break;
                case ConfigType_Unknown:
                    SetString(key, value);
                    break;
            }
            if (_configEventEnabled) Events::GetEvent("config:" + key)->Emit();
            return true;
        }
        
        ConfigType GetType(std::string key) {
            for (auto iter = _numberCvars.begin(); iter != _numberCvars.end(); iter++) {
                if (iter->first == key) {
                    return ConfigType_Number;
                }
            }
            for (auto iter = _boolCvars.begin(); iter != _boolCvars.end(); iter++) {
                if (iter->first == key) {
                    return ConfigType_Bool;
                }
            }
            for (auto iter = _stringCvars.begin(); iter != _stringCvars.end(); iter++) {
                if (iter->first == key) {
                    return ConfigType_String;
                }
            }
            return ConfigType_Unknown;
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
        
        UIConfigCollection GetAllUI() {
            UIConfigCollection ret;
            for (auto iter = _numberCvars.begin(); iter != _numberCvars.end(); iter++) {
                ret[iter->first] = {ConfigType_Number, std::to_string(iter->second)};
            }
            for (auto iter = _boolCvars.begin(); iter != _boolCvars.end(); iter++) {
                ret[iter->first] = {ConfigType_Bool, (iter->second ? "true" : "false")};
            }
            for (auto iter = _stringCvars.begin(); iter != _stringCvars.end(); iter++) {
                ret[iter->first] = {ConfigType_String, iter->second};
            }
            return ret;
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