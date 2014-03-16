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

#include "common.hpp"

#include "TextureLoader.hpp"
#include "SpriteSheet.hpp"

#define BUFFER_SIZE 4096

namespace Engine {
    
    class Texture;
    class SpriteSheet;
    
    namespace Draw2D {
        typedef struct _Color3f {
            float r, g, b;
        } Color3f;
        
        bool CheckGLError(const char* source);
        
        void SetDefinedColor(std::string name, int col);
        
        void ResetMatrix();
        
        void BeginRendering(GLenum mode);
        
        void EndRendering();
        
        void AddVert(float x, float y, float z);
        void AddVert(float x, float y, float z, double r, double g, double b, double a);
        void AddVert(float x, float y, float z, float s, float t);
        void AddVert(float x, float y, float z, double r, double g, double b, double a, float s, float t);
        
        void EnableTexture(Texture* texId);
        void DisableTexture();
        
        void EnableSmooth();
        void DisableSmooth();
        
        void Print(float x, float y, const char* string);
        float CalcStringWidth(std::string str);
        void SetFont(std::string name, int fontSize);
        void LoadFont(std::string prettyName, std::string filename);
        bool IsFontLoaded(std::string name);
        
        void FlushAll();
		
        void Init2d();
        
		void Begin2d();
		void End2d();
        
        void Reset();
        
        void ClearColor(int col);
        void ClearColor(std::string colorName);
        
        Color3f GetColor();
        
        void SetColor(int col);
        void SetColor(std::string colorName);
        void SetColor(float r, float g, float b);
        void SetColor(float r, float g, float b, float a);
        void GLSetColor();
        
        int GetVerts();
        
        void SetCenter(float x, float y);
        void CameraPan(float x, float y);
        void CameraZoom(float f);
        void CameraRotate(float r);
        
        void Rect(float x, float y, float w, float h);
        void Grid(float x, float y, float w, float h);
        
        void DrawImage(Texture* tex, float x, float y, float w, float h);
        void DrawImage(Texture* tex, float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
        void DrawSprite(SpriteSheet* sheet, std::string sprite, float x, float y, float w, float h);
        
        void Grad(float x, float y, float w, float h, int col1, int col2, bool vert);
        
        void Line(float x1, float y1, float x2, float y2);
        void Lines(float* points, unsigned int count);
        void LineGraph(float xOff, float yOff, float xScale, float yScale, float* points, unsigned int count);
        
        void Circle(float xCenter, float yCenter, float radius);
        void Circle(float xCenter, float yCenter, float radius, bool fill);
        void Circle(float xCenter, float yCenter, float radius, int segments, bool fill);
        void Circle(float xCenter, float yCenter, float radius, int segments, float start, float end, bool fill);
        void Circle(float xCenter, float yCenter, float radius, float innerRadius,
                    int segments, float start, float end, bool fill);
        
        void BezierCurve(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
        void BezierCurve(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int segments);
        void BezierCurve(glm::vec3 vec1, glm::vec3 vec2, glm::vec3 vec3, glm::vec3 vec4, int segments);
    }
}