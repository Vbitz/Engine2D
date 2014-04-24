/*
   Filename: Draw2D.cpp
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

#include "Draw2D.hpp"

#include "RenderGL3.hpp"

#include "Application.hpp"

#include "GL3Buffer.hpp"
#include "EffectParameters.hpp"

#include "vendor/GLFT_Font.hpp"

namespace Engine {
    void Draw2D::Rect(float x, float y, float w, float h) {
        RenderGL3::BeginRendering(GL_TRIANGLES);
            RenderGL3::AddVert(x, y, 0);
            RenderGL3::AddVert(x + w, y, 0);
            RenderGL3::AddVert(x + w, y + h, 0);
            RenderGL3::AddVert(x, y, 0);
            RenderGL3::AddVert(x, y + h, 0);
            RenderGL3::AddVert(x + w, y + h, 0);
        RenderGL3::EndRendering();
    }
    
    void Draw2D::Grid(float x, float y, float w, float h) {
        RenderGL3::BeginRendering(GL_LINE_LOOP);
            RenderGL3::AddVert(x, y, 0);
            RenderGL3::AddVert(x + w, y, 0);
            RenderGL3::AddVert(x + w, y + h, 0);
            RenderGL3::AddVert(x, y + h, 0);
            RenderGL3::AddVert(x, y, 0);
        RenderGL3::EndRendering();
    }
    
    void Draw2D::DrawImage(Texture* tex, float x, float y, float w, float h) {
        RenderGL3::EnableTexture(tex);
        
        RenderGL3::BeginRendering(GL_TRIANGLES);
        
        if (GetAppSingilton()->UsingGL3()) {
            RenderGL3::CheckGLError("Draw2D::DrawImage::PostStart"); // throws GL_INVALID_OPERATION when Begin turns into glBegin
        }
        
        //                 x         y         z   s     t
        RenderGL3::AddVert(x,        y,        0,  0.0f, 0.0f);
        RenderGL3::AddVert(x + w,    y,        0,  1.0f, 0.0f);
        RenderGL3::AddVert(x + w,    y + h,    0,  1.0f, 1.0f);
        RenderGL3::AddVert(x,        y,        0,  0.0f, 0.0f);
        RenderGL3::AddVert(x,        y + h,    0,  0.0f, 1.0f);
        RenderGL3::AddVert(x + w,    y + h,    0,  1.0f, 1.0f);
        
        RenderGL3::EndRendering();
        
        RenderGL3::CheckGLError("Draw2D::DrawImage::PostEnd");
        
        RenderGL3::DisableTexture();
        
        RenderGL3::CheckGLError("Draw2D::DrawImage::PostDraw");
    }
    
    void Draw2D::DrawImage(Texture* tex, float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
        RenderGL3::EnableTexture(tex);
        
        int imageWidth = tex->GetWidth();
        int imageHeight = tex->GetHeight();
        
        RenderGL3::BeginRendering(GL_TRIANGLES);
        //                 x           y           z       s                       t
        RenderGL3::AddVert(x1,         y1,         0,      x2 / imageWidth,        y2 / imageHeight);
        RenderGL3::AddVert(x1 + w1,    y1,         0,      (x2 + w2) / imageWidth, y2 / imageHeight);
        RenderGL3::AddVert(x1 + w1,    y1 + h1,    0,      (x2 + w2) / imageWidth, (y2 + h2) / imageHeight);
        RenderGL3::AddVert(x1,         y1,         0,      x2 / imageWidth,        y2 / imageHeight);
        RenderGL3::AddVert(x1,         y1 + h1,    0,      x2 / imageWidth,        (y2 + h2) / imageHeight);
        RenderGL3::AddVert(x1 + w1,    y1 + h1,    0,      (x2 + w2) / imageWidth, (y2 + h2) / imageHeight);
        
        RenderGL3::EndRendering();
        
        RenderGL3::DisableTexture();
    }
    
    void Draw2D::DrawSprite(SpriteSheet* sheet, std::string sprite, float x, float y, float w, float h) {
        Sprite s = sheet->GetSprite(sprite);
        this->DrawImage(s.tex, x, y, w, h, s.loc.x, s.loc.y, s.loc.w, s.loc.h);
    }
    
    void Draw2D::Grad(float x, float y, float w, float h, int col1, int col2, bool vert) {
        if (col1 > 256 * 256 * 256 || col2 > 256 * 256 * 256) {
            return;
        }
        
        Color4f color1 = RenderGL3::IntToColor(col1);
        
        Color4f color2 = RenderGL3::IntToColor(col2);
        
        RenderGL3::BeginRendering(GL_TRIANGLES);
        if (vert) {
            RenderGL3::AddVert(x, y, 0, color1);
            RenderGL3::AddVert(x + w, y, 0, color1);
            RenderGL3::AddVert(x + w, y + h, 0, color2);
            RenderGL3::AddVert(x, y, 0, color1);
            RenderGL3::AddVert(x, y + h, 0, color2);
            RenderGL3::AddVert(x + w, y + h, 0, color2);
        } else {
            RenderGL3::AddVert(x, y, 0, color1);
            RenderGL3::AddVert(x + w, y, 0, color2);
            RenderGL3::AddVert(x + w, y + h, 0, color2);
            RenderGL3::AddVert(x, y, 0, color1);
            RenderGL3::AddVert(x, y + h, 0, color1);
            RenderGL3::AddVert(x + w, y + h, 0, color2);
        }
        RenderGL3::EndRendering();
    }
    
    void Draw2D::Line(float x0, float y0, float x1, float y1) {
        RenderGL3::BeginRendering(GL_LINES);
            RenderGL3::AddVert(x0, y0, 0.0f);
            RenderGL3::AddVert(x1, y1, 0.0f);
        RenderGL3::EndRendering();
    }
    
    void Draw2D::Lines(float* points, unsigned int count) {
        RenderGL3::BeginRendering(GL_LINE_STRIP);
        for (int i = 0; i < count * 2; i += 2) {
            RenderGL3::AddVert(points[i], points[i + 1], 0);
        }
        RenderGL3::EndRendering();
    }
    
    void Draw2D::LineGraph(float xOff, float yOff, float xScale, float yScale, float* points, unsigned int count) {
        float* newPoints = (float*) std::malloc(count * 2 * sizeof(float)); // twice the size of points
        for (int x = 0; x < count; x++) {
            newPoints[x * 2] = xOff + (x * xScale);
            newPoints[x * 2 + 1] = yOff + ((-points[x]) * yScale);
        }
        this->Lines(newPoints, count);
        std::free(newPoints);
    }
    
void Draw2D::Circle(float xCenter, float yCenter, float radius) {
        this->Circle(xCenter, yCenter, radius, radius * 5, 0.0f, 1.0f, true);
    }
    
    void Draw2D::Circle(float xCenter, float yCenter, float radius, bool fill) {
        this->Circle(xCenter, yCenter, radius, radius * 5, 0.0f, 1.0f, fill);
    }
    
    void Draw2D::Circle(float xCenter, float yCenter, float radius, int segments, bool fill) {
        this->Circle(xCenter, yCenter, radius, segments, 0.0f, 1.0f, fill);
    }
    
    void Draw2D::Circle(float xCenter, float yCenter, float radius, int segments, float start, float end, bool fill) {
        this->Circle(xCenter, yCenter, radius, radius, segments, start, end, fill);
    }
    
    void Draw2D::Circle(float xCenter, float yCenter, float radius, float innerRadius,
                int segments, float start, float end, bool fill) {
        bool usingGl3 = GetAppSingilton()->UsingGL3();
        static double pi2 = 2 * 3.14159265358979323846;
        float rStart = pi2 * start;
        float rEnd = pi2 * end;
        float res = pi2 / segments;
        if (usingGl3) {
            RenderGL3::FlushAll();
        }
        RenderGL3::BeginRendering(fill ? (innerRadius != radius ? GL_TRIANGLE_STRIP // doughnut
                               : GL_TRIANGLE_FAN) // filled circle
                       : (end == 1.0f ? GL_LINE_LOOP // circle outline
                          : GL_LINE_STRIP // circle outline segment
                          ));
        if (fill) {
            if (innerRadius == radius) {
                RenderGL3::AddVert(xCenter, yCenter, 0.0f);
            } else {
                RenderGL3::AddVert(xCenter + radius * glm::cos(rStart), yCenter + radius * glm::sin(rStart), 0.0f);
                RenderGL3::AddVert(xCenter + innerRadius * glm::cos(rStart), yCenter + innerRadius * glm::sin(rStart), 0.0f);
            }
        }
        for (float i = rStart; i < rEnd; i += res) {
                                                RenderGL3::AddVert(xCenter + radius * glm::cos(i), yCenter + radius * glm::sin(i), 0.0f);
            if (innerRadius != radius && fill)  RenderGL3::AddVert(xCenter + innerRadius * glm::cos(i), yCenter + innerRadius * glm::sin(i), 0.0f);
        }
        if (fill && end == 1.0f && start == 0.0f) {
            if (innerRadius == radius) {
                RenderGL3::AddVert(xCenter + radius * glm::cos(rStart), yCenter + radius * glm::sin(rStart), 0.0f);
            } else {
                RenderGL3::AddVert(xCenter + radius * glm::cos(rStart), yCenter + radius * glm::sin(rStart), 0.0f);
                RenderGL3::AddVert(xCenter + innerRadius * glm::cos(rStart), yCenter + innerRadius * glm::sin(rStart), 0.0f);
            }
        }
        RenderGL3::EndRendering();
        if (usingGl3) {
            RenderGL3::FlushAll();
        }
    }
    
    
    // from http://devmag.org.za/2011/04/05/bzier-curves-a-tutorial/
    glm::vec3 _calculateBezierPoint(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
        float u = 1.0f - t;
        float tt = t*t;
        float uu = u*u;
        float uuu = uu * u;
        float ttt = tt * t;
        
        glm::vec3 p = uuu * p0; //first term
        p += 3 * uu * t * p1;   //second term
        p += 3 * u * tt * p2;   //third term
        p += ttt * p3;          //fourth term
        
        return p;
    }
    
    void Draw2D::BezierCurve(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
        glm::vec3 vec1 = glm::vec3(x1, y1, 0.0f);
        glm::vec3 vec2 = glm::vec3(x2, y2, 0.0f);
        glm::vec3 vec3 = glm::vec3(x3, y3, 0.0f);
        glm::vec3 vec4 = glm::vec3(x4, y4, 0.0f);
        
        float totalDistince = glm::distance(vec1, vec2) + glm::distance(vec2, vec3) + glm::distance(vec3, vec4);
        
        this->BezierCurve(vec1, vec2, vec3, vec4, glm::floor(totalDistince / 25));
    }
    
    void Draw2D::BezierCurve(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int segments) {
        glm::vec3 vec1 = glm::vec3(x1, y1, 0.0f);
        glm::vec3 vec2 = glm::vec3(x2, y2, 0.0f);
        glm::vec3 vec3 = glm::vec3(x3, y3, 0.0f);
        glm::vec3 vec4 = glm::vec3(x4, y4, 0.0f);
        this->BezierCurve(vec1, vec2, vec3, vec4, segments);
    }
    
    void Draw2D::BezierCurve(glm::vec3 vec1, glm::vec3 vec2, glm::vec3 vec3, glm::vec3 vec4, int segments) {
        glm::vec3 q0 = _calculateBezierPoint(0, vec1, vec2, vec3, vec4);
        
        RenderGL3::BeginRendering(GL_LINE_STRIP);
        
        RenderGL3::AddVert(q0.x, q0.y, q0.z);
        
        for (int i = 1; i <= segments; i++) {
            float t = (1 / (float) segments) * i;
            q0 = _calculateBezierPoint(t, vec1, vec2, vec3, vec4);
            RenderGL3::AddVert(q0.x, q0.y, q0.z);
        }
        RenderGL3::EndRendering();
    }
}