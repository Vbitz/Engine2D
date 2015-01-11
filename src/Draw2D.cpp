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

#include "RenderDriver.hpp"

#include "Profiler.hpp"

namespace Engine {
    void Draw2D::Rect(float x, float y, float w, float h) {
        ENGINE_PROFILER_SCOPE;
        renderGL->BeginRendering(PolygonMode::Triangles);
            renderGL->AddVert(x, y, 0);
            renderGL->AddVert(x + w, y, 0);
            renderGL->AddVert(x + w, y + h, 0);
            renderGL->AddVert(x, y, 0);
            renderGL->AddVert(x, y + h, 0);
            renderGL->AddVert(x + w, y + h, 0);
        renderGL->EndRendering();
    }
    
    void Draw2D::Grid(float x, float y, float w, float h) {
        ENGINE_PROFILER_SCOPE;
        renderGL->BeginRendering(PolygonMode::LineLoop);
            renderGL->AddVert(x, y, 0);
            renderGL->AddVert(x + w, y, 0);
            renderGL->AddVert(x + w, y + h, 0);
            renderGL->AddVert(x, y + h, 0);
            renderGL->AddVert(x, y, 0);
        renderGL->EndRendering();
    }
    
    void Draw2D::DrawImage(TexturePtr tex, float x, float y, float w, float h) {
        ENGINE_PROFILER_SCOPE;
        renderGL->EnableTexture(tex);
        
        renderGL->BeginRendering(PolygonMode::Triangles);
        
        if (renderGL->GetRendererType() == RendererType::OpenGL3) {
            renderGL->CheckError("Draw2D::DrawImage::PostStart"); // throws GL_INVALID_OPERATION when Begin turns into glBegin
        }
        
        //                 x         y         z   s     t
        renderGL->AddVert( x,        y,        0,  0.0f, 0.0f);
        renderGL->AddVert( x + w,    y,        0,  1.0f, 0.0f);
        renderGL->AddVert( x + w,    y + h,    0,  1.0f, 1.0f);
        renderGL->AddVert( x,        y,        0,  0.0f, 0.0f);
        renderGL->AddVert( x,        y + h,    0,  0.0f, 1.0f);
        renderGL->AddVert( x + w,    y + h,    0,  1.0f, 1.0f);
        
        renderGL->EndRendering();
        
        renderGL->CheckError("Draw2D::DrawImage::PostEnd");
        
        renderGL->DisableTexture();
        
        renderGL->CheckError("Draw2D::DrawImage::PostDraw");
    }
    
    void Draw2D::DrawImage(TexturePtr tex, float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
        ENGINE_PROFILER_SCOPE;
        renderGL->EnableTexture(tex);
        
        int imageWidth = tex->GetWidth();
        int imageHeight = tex->GetHeight();
        
        renderGL->BeginRendering(PolygonMode::Triangles);
        //                x           y           z       s                       t
        renderGL->AddVert(x1,         y1,         0,      x2 / imageWidth,        y2 / imageHeight);
        renderGL->AddVert(x1 + w1,    y1,         0,      (x2 + w2) / imageWidth, y2 / imageHeight);
        renderGL->AddVert(x1 + w1,    y1 + h1,    0,      (x2 + w2) / imageWidth, (y2 + h2) / imageHeight);
        renderGL->AddVert(x1,         y1,         0,      x2 / imageWidth,        y2 / imageHeight);
        renderGL->AddVert(x1,         y1 + h1,    0,      x2 / imageWidth,        (y2 + h2) / imageHeight);
        renderGL->AddVert(x1 + w1,    y1 + h1,    0,      (x2 + w2) / imageWidth, (y2 + h2) / imageHeight);
        
        renderGL->EndRendering();
        
        renderGL->DisableTexture();
    }
    
    void Draw2D::DrawSprite(SpriteSheetPtr sheet, std::string sprite, float x, float y, float w, float h) {
        ENGINE_PROFILER_SCOPE;
        Sprite s = sheet->GetSprite(sprite);
        this->DrawImage(s.tex, x, y, w, h, s.loc.x, s.loc.y, s.loc.w, s.loc.h);
    }
    
    void Draw2D::Grad(float x, float y, float w, float h, int col1, int col2, bool vert) {
        ENGINE_PROFILER_SCOPE;
        if (col1 > 256 * 256 * 256 || col2 > 256 * 256 * 256) {
            return;
        }
        
        Color4f color1(col1);
        
        Color4f color2(col2);
        
        renderGL->BeginRendering(PolygonMode::Triangles);
        if (vert) {
            renderGL->AddVert(x, y, 0, color1);
            renderGL->AddVert(x + w, y, 0, color1);
            renderGL->AddVert(x + w, y + h, 0, color2);
            renderGL->AddVert(x, y, 0, color1);
            renderGL->AddVert(x, y + h, 0, color2);
            renderGL->AddVert(x + w, y + h, 0, color2);
        } else {
            renderGL->AddVert(x, y, 0, color1);
            renderGL->AddVert(x + w, y, 0, color2);
            renderGL->AddVert(x + w, y + h, 0, color2);
            renderGL->AddVert(x, y, 0, color1);
            renderGL->AddVert(x, y + h, 0, color1);
            renderGL->AddVert(x + w, y + h, 0, color2);
        }
        renderGL->EndRendering();
    }
    
    void Draw2D::Line(float x0, float y0, float x1, float y1) {
        ENGINE_PROFILER_SCOPE;
        renderGL->BeginRendering(PolygonMode::Lines);
            renderGL->AddVert(x0, y0, 0.0f);
            renderGL->AddVert(x1, y1, 0.0f);
        renderGL->EndRendering();
    }
    
    void Draw2D::Lines(double* points, unsigned int count) {
        ENGINE_PROFILER_SCOPE;
        renderGL->BeginRendering(PolygonMode::Lines);
		for (int i = 0; i < (count - 1) * 2; i += 2) {
			renderGL->AddVert(points[i], points[i + 1], 0);
            renderGL->AddVert(points[i + 2], points[i + 3], 0);
        }
        renderGL->EndRendering();
    }
    
    void Draw2D::LineGraph(float xOff, float yOff, float xScale, float yScale, double* points, unsigned int count) {
        ENGINE_PROFILER_SCOPE;
        double* newPoints = (double*) std::malloc(count * 2 * sizeof(double)); // twice the size of points
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
    
    void Draw2D::Circle(float xCenter, float yCenter, float radius, float innerRadius, int segments, float start, float end, bool fill) {
        ENGINE_PROFILER_SCOPE;
        bool usingGl3 = renderGL->GetRendererType() == RendererType::OpenGL3;
        static double pi2 = 2 * 3.14159265358979323846;
        float rStart = pi2 * start;
        float rEnd = pi2 * end;
        float res = pi2 / segments;
        if (usingGl3) {
            renderGL->TrackStat(RenderStatistic::UserFlush, 1);
            renderGL->FlushAll();
        }
        renderGL->BeginRendering(fill ? (innerRadius != radius ? PolygonMode::TriangleStrip // doughnut
                               : PolygonMode::TriangleFan) // filled circle
                       : (end == 1.0f ? PolygonMode::LineLoop // circle outline
                          : PolygonMode::LineStrip // circle outline segment
                          ));
        if (fill) {
            if (innerRadius == radius) {
                renderGL->AddVert(xCenter, yCenter, 0.0f);
            } else {
                renderGL->AddVert(xCenter + radius * glm::cos(rStart), yCenter + radius * glm::sin(rStart), 0.0f);
                renderGL->AddVert(xCenter + innerRadius * glm::cos(rStart), yCenter + innerRadius * glm::sin(rStart), 0.0f);
            }
        }
        for (float i = rStart; i < rEnd; i += res) {
                renderGL->AddVert(xCenter + radius * glm::cos(i), yCenter + radius * glm::sin(i), 0.0f);
            if (innerRadius != radius && fill)
                renderGL->AddVert(xCenter + innerRadius * glm::cos(i), yCenter + innerRadius * glm::sin(i), 0.0f);
        }
        if (fill && end == 1.0f && start == 0.0f) {
            if (innerRadius == radius) {
                renderGL->AddVert(xCenter + radius * glm::cos(rStart), yCenter + radius * glm::sin(rStart), 0.0f);
            } else {
                renderGL->AddVert(xCenter + radius * glm::cos(rStart), yCenter + radius * glm::sin(rStart), 0.0f);
                renderGL->AddVert(xCenter + innerRadius * glm::cos(rStart), yCenter + innerRadius * glm::sin(rStart), 0.0f);
            }
        }
        renderGL->EndRendering();
        if (usingGl3) {
            renderGL->TrackStat(RenderStatistic::UserFlush, 1);
            renderGL->FlushAll();
        }
    }
    
    // points are in the format [x, y, x, y]
    void Draw2D::Polygon(float xCenter, float yCenter, float* points, int pointCount) {
        renderGL->BeginRendering(PolygonMode::TriangleFan);
        
        renderGL->AddVert(xCenter, yCenter, 0.0f);
        
        for (int i = 0; i < pointCount; i++) {
            renderGL->AddVert(xCenter + points[i * 2 + 0],
                              yCenter + points[i * 2 + 1],
                              0.0f);
        }
        
        renderGL->EndRendering();
        
        if (renderGL->GetRendererType() == RendererType::OpenGL3) {
            renderGL->TrackStat(RenderStatistic::UserFlush, 1);
            renderGL->FlushAll();
        }
    }
    
    // from http://devmag.org.za/2011/04/05/bzier-curves-a-tutorial/
    inline glm::vec2 _calculateBezierPoint(float t, const glm::vec2 &p0, const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec2 &p3) {
        const float u = 1.0f - t;
        
        return ((u * u * u) * p0) +        //first term
                (3 * (u * u) * t * p1) +   //second term
                (3 * u * (t * t) * p2) +   //third term
                ((t * t * t) * p3);        //fourth term
    }
    
    void Draw2D::BezierCurve(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
        glm::vec2 vec1 = glm::vec2(x1, y1);
        glm::vec2 vec2 = glm::vec2(x2, y2);
        glm::vec2 vec3 = glm::vec2(x3, y3);
        glm::vec2 vec4 = glm::vec2(x4, y4);
        
        float totalDistince = glm::distance(vec1, vec2) + glm::distance(vec2, vec3) + glm::distance(vec3, vec4);
        
        this->BezierCurve(vec1, vec2, vec3, vec4, glm::floor(totalDistince / 25));
    }
    
    void Draw2D::BezierCurve(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int segments) {
        glm::vec2 vec1 = glm::vec2(x1, y1);
        glm::vec2 vec2 = glm::vec2(x2, y2);
        glm::vec2 vec3 = glm::vec2(x3, y3);
        glm::vec2 vec4 = glm::vec2(x4, y4);
        
        this->BezierCurve(vec1, vec2, vec3, vec4, segments);
    }
    
    void Draw2D::BezierCurve(const glm::vec2 &vec1, const glm::vec2 &vec2,
                             const glm::vec2 &vec3, const glm::vec2 &vec4,
                             int segments) {
        ENGINE_PROFILER_SCOPE;
        glm::vec2 q0 = _calculateBezierPoint(0, vec1, vec2, vec3, vec4);
        glm::vec2 q1;
        
        renderGL->BeginRendering(PolygonMode::Lines);
        
        for (int i = 1; i <= segments; i++) {
            q1 = _calculateBezierPoint((1 / (float) segments) * i
                                       , vec1, vec2, vec3, vec4);
            renderGL->AddVert(q0.x, q0.y, 0.0);
            renderGL->AddVert(q1.x, q1.y, 0.0);
            q0 = q1;
        }
        
        renderGL->EndRendering();
    }
}