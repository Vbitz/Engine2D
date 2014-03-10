/*
   Filename: FramePerfMonitor.cpp
   Purpose:  Basic frame performance monitor for timing infomation

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

#include "FramePerfMonitor.hpp"

#include "Platform.hpp"
#include <math.h>

namespace Engine {
	namespace FramePerfMonitor {
        double  _startTime = -1.0,
                _rawFrameTime = -1.0,
                _fpsTimer = 0.0,
                _currentFPS = 0.0;
        
        void BeginFrame() {
            _startTime = Platform::GetTime();
        }
        
		void EndFrame() {
            _rawFrameTime = Platform::GetTime() - _startTime;
            
            float fps = 1000 / (_rawFrameTime * 1000);
            
            _fpsTimer += _rawFrameTime;
            
            if (_fpsTimer > 0.1f) {
                _currentFPS = floor(fps);
                _fpsTimer = 0;
            }
        }

		double GetFrameTime() {
            return _rawFrameTime;
        }
        
        double GetFPS() {
            return _currentFPS;
        }
	}
}