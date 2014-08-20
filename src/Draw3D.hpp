/*
   Filename: Draw3D.hpp
   Purpose:  3D Primitive Drawing and Management

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

#include "stdlib.hpp"
#include "RenderDriver.hpp"

   
#define GLM_FORCE_RADIANS
#include "vendor/glm/glm.hpp"

namespace Engine {
    ENGINE_CLASS(Draw3D);
    
    class Draw3D {
    public:
        Draw3D(RenderDriverPtr renderGL) : renderGL(renderGL) {}
        
        inline RenderDriverPtr GetRender() {
            return renderGL;
        }
        
        void Begin();
        void End();
        
        void Cube(glm::vec3 p1, glm::vec3 p2);
    private:
        RenderDriverPtr renderGL = NULL;
    };
}