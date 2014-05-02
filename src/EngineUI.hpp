/*
   Filename: EngineUI.hpp
   Purpose:  Draws the EngineUI and console on top of rendering

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

#include "Application.hpp"
#include "Draw2D.hpp"

namespace Engine {
    class Application;
    
    class EngineUI {
    public:
        EngineUI(Application* app);
        
        void Draw();
        void OnKeyPress(int key, int press, bool shift);
        
        void SetActive(bool active);
        
        void ToggleConsole();
        
        void ClearConsole();
        
        bool ConsoleActive();
    private:
        Application* _app;
        Draw2D* _draw;
        
        std::stringstream _ss;
        
        std::stringstream _currentConsoleInput;
        
        bool _active = true;
        bool _showConsole = false;
        
        // just temporey until the profiler has this built in
        float _lastDrawTimes[100];
        int _currentLastDrawTimePos = 0;
        
        std::vector<std::string> _commandHistory;
        size_t _currentHistoryLine = 0;
    };
}