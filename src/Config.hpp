/*
   Filename: Config.hpp
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

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace Engine {
    namespace Config {
        enum ConfigType {
            ConfigType_String,
            ConfigType_Bool,
            ConfigType_Number,
            ConfigType_Unknown
        };
        
        struct ConfigValue {
            ConfigType type;
            std::string key;
            std::string value;
        };
        
        void EnableConfigEvents();
        
        void SetNumber(std::string key, int value);
        void SetNumber(std::string key, float value);
        void SetBoolean(std::string key, bool value);
        void SetString(std::string key, std::string value);
        
        bool Set(std::string key, std::string value);
        
        ConfigType GetType(std::string key);
        std::string Get(std::string key);
        int GetInt(std::string key);
        float GetFloat(std::string key);
        bool GetBoolean(std::string key);
        std::string GetString(std::string key);
        
        std::vector<std::string> GetAll();
        std::vector<ConfigValue> GetAllUI();
    }
}