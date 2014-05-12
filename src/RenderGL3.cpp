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
    std::string GLErrorString(GLenum error) {
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
    
    std::string GLModeToString(GLenum mode) {
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
            return GetOpenGLVersion().major >= 3 ? RendererType_OpenGL3 : RendererType_OpenGL2;
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
            
            if (GetRendererType() == RendererType_OpenGL3) {
                glGetError(); // A bug in GLEW always throws a GL error
            }
            
            CheckError("RenderGL3::HasExtention::PostGlewGetExtention");
            
            return has;
        }
        
        std::vector<std::string> GetExtentions() override {
            std::vector<std::string> ret;
            
            if (GetRendererType() == RendererType_OpenGL3) {
                int extentionCount;
                glGetIntegerv(GL_NUM_EXTENSIONS, &extentionCount);
                
                for (int i = 0; i < extentionCount; i++) {
                    const GLubyte* str = glGetStringi(GL_EXTENSIONS, i);
                    if (str != NULL) {
                        ret.push_back(std::string((const char*) str));
                    }
                }
            } else {
                std::string extentionList = std::string((const char*) glGetString(GL_EXTENSIONS));
                size_t currentPos = 0;
                int currentIndex = 0;
                while (currentPos < extentionList.length() && currentPos != std::string::npos) {
                    size_t nextPos = extentionList.find(' ', currentPos);
                    if (nextPos == std::string::npos) {
                        break;
                    }
                    ret.push_back(extentionList.substr(currentPos, nextPos - currentPos));
                    currentPos = extentionList.find(' ', currentPos) + 1;
                }
            }
            
            CheckError("RenderGL3::GetExtentions");
            
            return ret;
        }
        
        void ResetMatrix() override {
            if (GetRendererType() == RendererType_OpenGL3) {
                this->_currentModelMatrix = glm::mat4();
            } else {
                //glPushMatrix();
                glDisable(GL_DEPTH_TEST);
                glLoadIdentity();
                Application* app = GetAppSingilton();
                glOrtho(0, app->GetScreenWidth(), app->GetScreenHeight(), 0, 1, -1);
            }
        }
        
        void BeginRendering(int mode) override {
            if (GetRendererType() == RendererType_OpenGL3) {
                if (this->_currentMode != mode ||
                    this->_currentMode == GL_LINE_STRIP) {
                    // it's a hack, I really need to fix this
                    FlushAll();
                    this->_currentMode = mode;
                }
            } else {
                glBegin(mode);
            }
        }
        
        void EndRendering() override {
            if (GetRendererType() == RendererType_OpenGL3) {
                if (this->_currentVerts > BUFFER_SIZE - 256) {
                    FlushAll();
                    BeginRendering(_currentMode);
                }
            } else {
                glEnd();
            }
        }
        
        void EnableTexture(Texture* texId) override {
            if (GetRendererType() == RendererType_OpenGL3) {
                FlushAll();
                this->_currentTexture = texId;
            } else {
                glEnable(GL_TEXTURE_2D);
                
                texId->Begin();
            }
        }
        
        void DisableTexture() override {
            if (GetRendererType() == RendererType_OpenGL3) {
                FlushAll();
                _currentTexture = _defaultTexture;
            } else {
                glBindTexture(GL_TEXTURE_2D, 0);
                
                glDisable(GL_TEXTURE_2D);
            }
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
        
        void Print(float x, float y, const char* string) override {
            if (GetRendererType() == RendererType_OpenGL3) {
                FlushAll();
                
                GLFT_Font* drawingFont = GetAppSingilton()->GetFont(this->_currentFontName, this->_currentFontSize);
                
                drawingFont->drawTextGL3(x - this->_centerX, y - this->_centerY,
                                         this, this->_currentEffect,
                                         this->_currentColor.r, this->_currentColor.g, this->_currentColor.b, string);
                
                CheckError("RenderGL3::Print::PostGL3Print");
                
            } else {
                glEnable(GL_TEXTURE_2D);
                
                glColor4f(_currentColor.r, _currentColor.g, _currentColor.b, _currentColor.a);
                
                GLFT_Font* drawingFont = GetAppSingilton()->GetFont(_currentFontName, _currentFontSize);
                
                drawingFont->drawText(x - _centerX, y - _centerY, string);
                
                glDisable(GL_TEXTURE_2D);
                
                CheckError("RenderGL3::Print::PostGL2Print");
            }
        }
        
        void FlushAll() override {
            static GL3Buffer buf(this, this->_currentEffect); // temporory
            
            if (GetRendererType() != RendererType_OpenGL3) {
                throw "That's a bug";
            }
            
            if (_currentVerts == 0) {
                return; // nothing to draw
            }
            
            if (buf.Update()) {
                Logger::begin("RenderGL3", Logger::LogLevel_Log) << "Render Buffer Reloaded" << Logger::end();
            }
            
            try {
                _currentTexture->Begin();
            } catch (...) {
                _currentTexture->End();
                _currentTexture = _defaultTexture;
                _currentTexture->Begin();
            }
            buf.Upload((float*) _buffer, _currentVerts * sizeof(BufferFormat));
            //std::cout << "Drawing Using: " << GLModeToString(_currentMode) << std::endl;
            buf.Draw(_currentMode, _currentModelMatrix, glm::mat4(), _currentVerts);
            
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
            if (GetRendererType() == RendererType_OpenGL3) {
                std::string gl3Effect = Config::GetString("core.render.basicEffect");
                this->_currentEffect = EffectReader::GetEffectFromFile(gl3Effect);
                this->_currentEffect->CreateShader();
                if (this->_defaultTexture == NULL || !this->_defaultTexture->IsValid()) {
                    Logger::begin("RenderGL3", Logger::LogLevel_Verbose) << "Creating Default Texture" << Logger::end();
                    this->_defaultTexture = GenerateDefaultTexture();
                }
                this->_currentTexture = this->_defaultTexture;
            }
        }
        
		void Begin2d() override {
            EnableSmooth();
            
            if (GetRendererType() == RendererType_OpenGL3) {
                this->_currentTexture = this->_defaultTexture;
                glDisable(GL_DEPTH_TEST);
            } else {
                glDisable(GL_TEXTURE_2D);
            }
            
            ResetMatrix();
            
            CheckError("RenderGL3::Begin2d");
        }
        
		void End2d() override {
            if (GetRendererType() == RendererType_OpenGL3) {
                FlushAll();
            } else {
                //glPopMatrix();
            }
            
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
            if (GetRendererType() == RendererType_OpenGL3) {
                FlushAll();
                this->_currentModelMatrix =
                glm::translate(this->_currentModelMatrix, glm::vec3(x, y, 0.0f));
            } else {
                glTranslatef(x, y, 0.0f);
            }
        }
        
        void CameraZoom(float f) override {
            if (GetRendererType() == RendererType_OpenGL3) {
                FlushAll();
                this->_currentModelMatrix =
                glm::scale(_currentModelMatrix, glm::vec3(f, f, 0.0f));
            } else {
                glScalef(f, f, 0.0f);
            }
        }
        
        void CameraRotate(float r) override {
            if (GetRendererType() == RendererType_OpenGL3) {
                FlushAll();
                this->_currentModelMatrix =
                glm::rotate(this->_currentModelMatrix, r, glm::vec3(0, 0, 1));
            } else {
                glRotatef(r, 0.0f, 0.0f, 1.0f);
            }
        }
    protected:
        void _clearColor(Color4f col) override {
            glClearColor(col.r, col.g, col.b, col.a);
        }
        
        void _addVert(float x, float y, float z, Color4f col, float s, float t) override {
            if (GetRendererType() == RendererType_OpenGL3) {
                _buffer[_currentVerts].pos = glm::vec3(x - _centerX, y - _centerY, z);
                _buffer[_currentVerts].col = col;
                _buffer[_currentVerts].uv = glm::vec2(s, t);
                _currentVerts++;
            } else {
                glColor4f(col.r, col.g, col.b, col.a);
                glTexCoord2f(s, t);
                glVertex3f(x - _centerX, y - _centerY, z);
            }
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
        
        GLenum _currentMode = 0;
        
        EffectParameters* _currentEffect;
        
        Texture* _defaultTexture;
        
        Texture* _currentTexture = NULL;
        
        glm::mat4 _currentModelMatrix;;
    };
    
    RenderDriver* CreateRenderGL3() {
        return new RenderGL3();
    }
}