#pragma once

#include "common.hpp"

namespace Engine {
    namespace EngineUI {
        void Draw();
        void OnKeyPress(int key, bool press);
        
        void SetActive(bool active);
    }
}