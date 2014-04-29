#pragma once

#include "common.hpp"
#include "stdlib.hpp"

#include "TextureLoader.hpp"
#include "GL3Buffer.hpp"

#define BUFFER_SIZE 4096

namespace Engine {
    class Texture;
    class GL3Buffer;
    
    enum RendererType {
        RendererType_OpenGL3,
        RendererType_OpenGL2
    };
    
    class RenderGL3 {
    public:
        class GLError {
        public:
            GLenum Error;
            std::string Source, ErrorString;
            
            GLError(const char* source, GLenum err, std::string errorString) : Source(source), Error(err), ErrorString(errorString) {
                
            }
        };
        
        RendererType GetRendererType();
        OpenGLVersion GetOpenGLVersion();
        EffectShaderTypes::Type GetBestEffectShaderType();
        
        bool CheckGLError(const char* source);
        
        bool HasExtention(std::string extentionName);
        std::vector<std::string> GetExtentions();
        
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
    private:
        bool _drawOffScreen = true;
		
		Color4f _currentColor = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
        
        int _polygons = 0;
        int _drawCalls = 0;
        
        int _centerX = 0;
        int _centerY = 0;
        
        int _currentVerts = 0;
        
        /*
         Buffer format
         (x, y, z)      Position
         (r, g, b, a)   Color
         (u, v)         TexCourd
         */
        
#pragma pack(0)
        struct BufferFormat {
            glm::vec3 pos;
            Color4f col;
            glm::vec2 uv;
        };
        
        BufferFormat _buffer[BUFFER_SIZE];
        
        GLenum _currentMode = 0;
        
        std::string _currentFontName = "basic"; // ah there we are fonts fixed
        int _currentFontSize = 16;
        
        EffectParameters _currentEffect;
        
        Texture* _defaultTexture;
        
        Texture* _currentTexture = NULL;
        
        glm::mat4 _currentModelMatrix;;
    };
    
    RenderGL3* GetRenderGL();
}