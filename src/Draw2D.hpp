#pragma once

#include "common.hpp"
#include "main.hpp"

#include "extern/GLFT_Font.hpp"

#define BUFFER_SIZE 256

namespace Engine {
    namespace Draw2D {
        bool CheckGLError(std::string source);
        
        void ResetMatrix();
        
        bool IsOffscreen(int x, int y, int w, int h);
        
        void BeginRendering(GLenum mode);
        
        void EndRendering();
        
        void AddVert(float x, float y, float z);
        void AddVert(float x, float y, float z, double r, double g, double b, double a);
        void AddVert(float x, float y, float z, float s, float t);
        void AddVert(float x, float y, float z, double r, double g, double b, double a, float s, float t);
        
        void EnableTexture(int texId);
        void DisableTexture();
        
        void Print(float x, float y, const char* string);
        float CalcStringWidth(std::string str);
        void SetFont(std::string name, int fontSize);
        void LoadFont(std::string prettyName, std::string filename);
        bool IsFontLoaded(std::string name);
        
        bool IsValidTextureID(int texID);
        
        void FlushAll();
		
		void Begin2d();
		void End2d();
        
        void Reset();
        
        void SetColor(float r, float g, float b);
        void SetColor(float r, float g, float b, float a);
        void GLSetColor();
        
        int GetVerts();
        
        void SetDrawOffscreen(bool drawOffscreen);
        
        void SetCenter(float x, float y);
        
        void Rect(float x, float y, float w, float h);
        void Grid(float x, float y, float w, float h);
    }
}