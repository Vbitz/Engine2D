#pragma once

#include "common.hpp"
#include "stdlib.hpp"

#include "TextureLoader.hpp"

#define BUFFER_SIZE 4096

namespace Engine {
    class Texture;
    
    namespace RenderGL3 {
        class GLError {
        public:
            GLenum Error;
            std::string Source, ErrorString;
            
            GLError(const char* source, GLenum err, std::string errorString) : Source(source), Error(err), ErrorString(errorString) {
                
            }
        };
        
        bool CheckGLError(const char* source);
        
        void SetDefinedColor(std::string name, int col);
        
        void ResetMatrix();
        
        void BeginRendering(GLenum mode);
        
        void EndRendering();
        
        void AddVert(float x, float y, float z);
        void AddVert(float x, float y, float z, Color4f col);
        void AddVert(float x, float y, float z, float s, float t);
        void AddVert(float x, float y, float z, Color4f col, float s, float t);
        
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
        
        Color4f IntToColor(int col);
        
        void ClearColor(Color4f col);
        void ClearColor(int col);
        void ClearColor(std::string colorName);
        void ClearColor(float r, float g, float b);
        
        Color4f GetColor();
        
        void SetColor(Color4f col);
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
    }
}