/*
   Filename: RenderGL3.cpp
   Purpose:  OpenGL 3.x Rendering Interface

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

#define GLEW_STATIC
#include "vendor/GL/glew.h"

#include "RenderGL3.hpp"

#include "GL3Buffer.hpp"
#include "TextureLoader.hpp"

#include "Config.hpp"

#include "Application.hpp"

// TODO: define GLM_FORCE_RADIANS, I need to make sure that Draw2D.rotateCamera does this
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define BUFFER_SIZE 4096

namespace Engine {
    std::string GLErrorString(int error) {
        switch (error) {
            case GL_INVALID_ENUM:       return "Invalid Enum";
            case GL_INVALID_VALUE:      return "Invalid Value";
            case GL_INVALID_OPERATION:  return "Invalid Operation";
            case GL_OUT_OF_MEMORY:      return "Out of Memory";
            case GL_STACK_OVERFLOW:     return "Stack Overflow";
            case GL_STACK_UNDERFLOW:    return "Stack Underflow";
            default:                    return "Unknown Error: " + std::to_string(error);
        }
    }
    
    std::string GLModeToString(int mode) {
        switch (mode) {
            case GL_POINTS:         return "GL_POINTS";
            case GL_LINE_STRIP:     return "GL_LINE_STRIP";
            case GL_LINE_LOOP:      return "GL_LINE_LOOP";
            case GL_LINES:          return "GL_LINES";
            case GL_TRIANGLE_STRIP: return "GL_TRIANGLE_STRIP";
            case GL_TRIANGLE_FAN:   return "GL_TRIANGLE_FAN";
            case GL_TRIANGLES:      return "GL_TRIANGLES";
            case GL_QUAD_STRIP:     return "GL_QUAD_STRIP";
            case GL_QUADS:          return "GL_QUADS";
            case GL_POLYGON:        return "GL_POLYGON";
            default:                return "Unknown";
        }
    }
    
    Texture* GenerateDefaultTexture() {
        return ImageReader::TextureFromBuffer(new float[4] {1.0f, 1.0f, 1.0f, 1.0f}, 1, 1);
    }
    
    class RenderGL3 : public RenderDriver {
    public:
        RendererType GetRendererType() override {
            return RendererType_OpenGL3;
        }
        
        OpenGLVersion GetOpenGLVersion() override {
            return GetAppSingilton()->GetWindow()->GetGlVersion();
        }
        
        EffectShaderTypes::Type GetBestEffectShaderType() override {
            return EffectShaderTypes::GLSL_150;
        }
        
        bool CheckError(const char* source) override {
            GLenum err;
            bool oneError = false;
            while ((err = glGetError()) != GL_NO_ERROR) {
                Logger::begin("OpenGL", Logger::LogLevel_Error) << "GLError in " << source << " : " << GLErrorString(err) << Logger::end();
                throw new RenderDriverError(source, err, GLErrorString(err));
            }
            return oneError;
        }
        
        bool HasExtention(std::string extentionName) override {
            bool has = glewGetExtension(extentionName.c_str());
            
            glGetError(); // A bug in GLEW always throws a GL error
            
            CheckError("RenderGL3::HasExtention::PostGlewGetExtention");
            
            return has;
        }
        
        std::vector<std::string> GetExtentions() override {
            std::vector<std::string> ret;
            
            int extentionCount;
            glGetIntegerv(GL_NUM_EXTENSIONS, &extentionCount);
                
            for (int i = 0; i < extentionCount; i++) {
                const GLubyte* str = glGetStringi(GL_EXTENSIONS, i);
                if (str != NULL) {
                    ret.push_back(std::string((const char*) str));
                }
            }
            
            CheckError("RenderGL3::GetExtentions");
            
            return ret;
        }
        
        void ResetMatrix() override {
            this->_currentModelMatrix = glm::mat4();
            if (Config::GetBoolean("core.render.halfPix")) {
                this->_currentModelMatrix = glm::translate(this->_currentModelMatrix, glm::vec3(0.5f, 0.5f, 0.0f));
            }
        }
        
        void BeginRendering(int mode) override {
            if (this->_currentMode != mode ||
                this->_currentMode == GL_LINE_STRIP) {
                // it's a hack, I really need to fix this
                FlushAll();
                this->_currentMode = mode;
            }
        }
        
        void EndRendering() override {
            if (this->_currentVerts > BUFFER_SIZE - 256) {
                FlushAll();
                BeginRendering(_currentMode);
            }
        }
        
        void EnableTexture(Texture* texId) override {
            FlushAll();
            this->_currentTexture = texId;
        }
        
        void DisableTexture() override {
            FlushAll();
            _currentTexture = _defaultTexture;
        }
        
        void EnableSmooth() override {
            glEnable(GL_LINE_SMOOTH);
            glEnable(GL_POLYGON_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
        }
        
        void DisableSmooth() override {
            glDisable(GL_LINE_SMOOTH);
            glDisable(GL_POLYGON_SMOOTH);
        }
        
        void FlushAll() override {
            if (this->_gl3Buffer == NULL) {
                this->_gl3Buffer = new GL3Buffer(this, this->_currentEffect);
            }
            
            if (_currentVerts == 0) {
                return; // nothing to draw
            }
            
            if (this->_gl3Buffer->Update()) {
                Logger::begin("RenderGL3", Logger::LogLevel_Log) << "Render Buffer Reloaded" << Logger::end();
            }
            
            try {
                _currentTexture->Begin();
            } catch (...) {
                _currentTexture->End();
                _currentTexture = _defaultTexture;
                _currentTexture->Begin();
            }
            this->_gl3Buffer->Upload((float*) _buffer, _indexBuffer, _currentVerts, sizeof(BufferFormat));
            //std::cout << "Drawing Using: " << GLModeToString(_currentMode) << std::endl;
            this->_gl3Buffer->Draw(_currentMode, _currentModelMatrix, glm::mat4(), _currentVerts);
            
            _currentTexture->End();
            
            /* I think this line here needs a story. What happens when you forget it?
             Well as it turns out it will eat though the buffer growing ever bigger until
             it starts ruining C++ like a fucking pacman. I got 2 lovely crashes because I
             forgot this line. The first was caused when any std::unordered_map tries to
             rehash somehing, the second was the slow and painful death as a std::vector
             exceptions it's self to death over the span of 3 frames. This was my best line
             that day.
             */
            _currentVerts = 0;
            
            // maybe zero the buffer
        }
		
        void Init2d() override {
            std::string gl3Effect = Config::GetString("core.render.basicEffect");
            this->_currentEffect = EffectReader::GetEffectFromFile(gl3Effect);
            this->_currentEffect->CreateShader();
            if (this->_defaultTexture == NULL || !this->_defaultTexture->IsValid()) {
                Logger::begin("RenderGL3", Logger::LogLevel_Verbose) << "Creating Default Texture" << Logger::end();
                this->_defaultTexture = GenerateDefaultTexture();
            }
            this->_currentTexture = this->_defaultTexture;
        }
        
		void Begin2d() override {
            EnableSmooth();
            
            this->_currentTexture = this->_defaultTexture;
            glDisable(GL_DEPTH_TEST);
            
            ResetMatrix();
            
            CheckError("RenderGL3::Begin2d");
        }
        
		void End2d() override {
            FlushAll();
            
            CheckError("RenderGL3::End2d");
        }
        
        void Reset() override {
            End2d();
            Begin2d();
        }
        
        void SetCenter(float x, float y) override {
            this->_centerX = x;
            this->_centerY = y;
        }
        
        void CameraPan(float x, float y) override {
            FlushAll();
            this->_currentModelMatrix = glm::translate(this->_currentModelMatrix, glm::vec3(x, y, 0.0f));
        }
        
        void CameraZoom(float f) override {
            FlushAll();
            this->_currentModelMatrix =
            glm::scale(_currentModelMatrix, glm::vec3(f, f, 0.0f));
        }
        
        void CameraRotate(float r) override {
            FlushAll();
            this->_currentModelMatrix =
            glm::rotate(this->_currentModelMatrix, r, glm::vec3(0, 0, 1));
        }
    protected:
        void _clearColor(Color4f col) override {
            glClearColor(col.r, col.g, col.b, col.a);
        }
        
        void _addVert(float x, float y, float z, Color4f col, float s, float t) override {
            _buffer[_currentVerts].pos = glm::vec3(x - _centerX, y - _centerY, z);
            _buffer[_currentVerts].col = col;
            _buffer[_currentVerts].uv = glm::vec2(s, t);
            _indexBuffer[_currentVerts] = _currentVerts;
            _currentVerts++;
        }
        
        void _printFT(float x, float y, const char* string) override {
            FlushAll();
            
            GLFT_Font* drawingFont = GetAppSingilton()->GetFont(this->_currentFontName, this->_currentFontSize);
            
            drawingFont->drawTextGL3(x - this->_centerX, y - this->_centerY,
                                     this, this->_currentEffect,
                                     this->_currentColor.r, this->_currentColor.g, this->_currentColor.b, string);
            
            CheckError("RenderGL3::Print::PostGL3Print");
        }
        
    private:
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
        ushort _indexBuffer[BUFFER_SIZE];
        
        GLenum _currentMode = 0;
        
        EffectParameters* _currentEffect;
        
        Texture* _defaultTexture = NULL;
        
        Texture* _currentTexture = NULL;
        
        GL3Buffer* _gl3Buffer = NULL;
        
        glm::mat4 _currentModelMatrix;;
    };
    
    RenderDriver* CreateRenderGL3() {
        return new RenderGL3();
    }
}