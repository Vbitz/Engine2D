#pragma once

#include <string>
#include <map>

namespace Engine {
    namespace Config {
        void Set(std::string key, int value);
        void Set(std::string key, float value);
        void Set(std::string key, bool value);
        void Set(std::string key, std::string value);
        
        int GetInt(std::string key);
        float GetFloat(std::string key);
        bool GetBoolean(std::string key);
        std::string GetString(std::string key);
    }
}