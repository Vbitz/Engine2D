#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace Engine {
    namespace Config {
        void SetNumber(std::string key, int value);
        void SetNumber(std::string key, float value);
        void SetBoolean(std::string key, bool value);
        void SetString(std::string key, std::string value);
        
        std::string Get(std::string key);
        int GetInt(std::string key);
        float GetFloat(std::string key);
        bool GetBoolean(std::string key);
        std::string GetString(std::string key);
        
        std::vector<std::string> GetAll();
    }
}