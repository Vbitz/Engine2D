#pragma once

#include "common.hpp"

#include "main.hpp"

#include "Draw2D.hpp"

namespace Engine {
    namespace EngineUI {
        void Draw();
        void OnKeyPress(int key, bool press);
        
        void SetActive(bool active);
    }
}