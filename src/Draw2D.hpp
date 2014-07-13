/*
   Filename: Draw2D.hpp
   Purpose:  Drawing interface for basic 2D primatives and drawing management

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

#include "RenderDriver.hpp"

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

#include "SpriteSheet.hpp"

#define BUFFER_SIZE 4096

namespace Engine {
    ENGINE_CLASS(SpriteSheet);
    
    ENGINE_CLASS(Draw2D);
    
    class Draw2D {
    public:
        Draw2D(RenderDriverPtr renderGL) : renderGL(renderGL) {}
        
        RenderDriverPtr GetRender() {
            return renderGL;
        }
        
        // Primatives
        void Rect(float x, float y, float w, float h);
        void Grid(float x, float y, float w, float h);
        
        void DrawImage(TexturePtr tex, float x, float y, float w, float h);
        void DrawImage(TexturePtr tex, float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
        void DrawSprite(SpriteSheetPtr sheet, std::string sprite, float x, float y, float w, float h);
        
        void Grad(float x, float y, float w, float h, int col1, int col2, bool vert);
        
        void Line(float x1, float y1, float x2, float y2);
        void Lines(double* points, unsigned int count);
        void LineGraph(float xOff, float yOff, float xScale, float yScale, double* points, unsigned int count);
        
        void Circle(float xCenter, float yCenter, float radius);
        void Circle(float xCenter, float yCenter, float radius, bool fill);
        void Circle(float xCenter, float yCenter, float radius, int segments, bool fill);
        void Circle(float xCenter, float yCenter, float radius, int segments, float start, float end, bool fill);
        void Circle(float xCenter, float yCenter, float radius, float innerRadius,
                    int segments, float start, float end, bool fill);
        
        void Polygon(float xOff, float yCenter, float* points, int pointCount);
        
        void BezierCurve(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
        void BezierCurve(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int segments);
        void BezierCurve(glm::vec3 vec1, glm::vec3 vec2, glm::vec3 vec3, glm::vec3 vec4, int segments);
    private:
        RenderDriverPtr renderGL;
    };
}