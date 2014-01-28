#pragma once

#include <string>

namespace Engine {
    namespace Timer {
        void Update();
        
        int Create(double time, std::string event);
        int Create(double time, std::string event, bool repeat);
        void Remove(int timer);
    }
}