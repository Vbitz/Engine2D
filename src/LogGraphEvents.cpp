/*
   Filename: LogGraphEvents.cpp
   Purpose:  Not used anymore

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

#include "LogGraphEvents.hpp"

namespace Engine {
    namespace LogGraphEvents {
        int TestingEvent::Run(int x, int y) {
            RenderGL3::SetColor("white");
            Draw2D::Rect(x + 10, y - 30, 20, 20);
            return 40;
        }
    }
}