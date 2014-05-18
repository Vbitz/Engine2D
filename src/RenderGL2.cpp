/*
 Filename: RenderGL2.cpp
 Purpose:  OpenGL 2.x Rendering Interface
 
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

#include "RenderGL2.hpp"
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
    class RenderGL2 : public RenderDriver {
    public:
        RendererType GetRendererType() override {
            return RendererType_OpenGL2;
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
            
            CheckError("RenderGL2::HasExtention::PostGlewGetExtention");
            
            return has;
        }
        
        std::vector<std::string> GetExtentions() override {
            std::vector<std::string> ret;
            
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
            
            CheckError("RenderGL2::GetExtentions");
            
            return ret;
        }
        
        void ResetMatrix() override {
            //glPushMatrix();
            glDisable(GL_DEPTH_TEST);
            glLoadIdentity();
            Application* app = GetAppSingilton();
            glm::vec2 windowSize = app->GetWindow()->GetWindowSize();
            glOrtho(0, windowSize.x, windowSize.y, 0, 1, -1);
            if (Config::GetBoolean("core.render.halfPix")) {
                glTranslatef(0.5f, 0.5f, 0.0f);
            }
        }
        
        void BeginRendering(int mode) override {
            glBegin(mode);
        }
        
        void EndRendering() override {
            glEnd();
        }
        
        void EnableTexture(Texture* texId) override {
            glEnable(GL_TEXTURE_2D);
                
            texId->Begin();
        }
        
        void DisableTexture() override {
            glBindTexture(GL_TEXTURE_2D, 0);
                
            glDisable(GL_TEXTURE_2D);
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
            glEnable(GL_TEXTURE_2D);
                
            glColor4f(_currentColor.r, _currentColor.g, _currentColor.b, _currentColor.a);
                
            GLFT_Font* drawingFont = GetAppSingilton()->GetFont(_currentFontName, _currentFontSize);
                
            drawingFont->drawText(x - _centerX, y - _centerY, string);
                
            glDisable(GL_TEXTURE_2D);
                
            CheckError("RenderGL2::Print::PostGL2Print");
        }
        
        void FlushAll() override { }
		
        void Init2d() override { }
        
		void Begin2d() override {
            EnableSmooth();
            
            glDisable(GL_TEXTURE_2D);
            
            ResetMatrix();
            
            CheckError("RenderGL2::Begin2d");
        }
        
		void End2d() override {
            CheckError("RenderGL2::End2d");
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
            glTranslatef(x, y, 0.0f);
        }
        
        void CameraZoom(float f) override {
            glScalef(f, f, 0.0f);
        }
        
        void CameraRotate(float r) override {
            glRotatef(r, 0.0f, 0.0f, 1.0f);
        }
    protected:
        void _clearColor(Color4f col) override {
            glClearColor(col.r, col.g, col.b, col.a);
        }
        
        void _addVert(float x, float y, float z, Color4f col, float s, float t) override {
            glColor4f(col.r, col.g, col.b, col.a);
            glTexCoord2f(s, t);
            glVertex3f(x - _centerX, y - _centerY, z);
        }
    private:
        int _centerX = 0;
        int _centerY = 0;
        
        int _currentVerts = 0;
        
        GLenum _currentMode = 0;
        
        EffectParameters* _currentEffect;
        
        Texture* _defaultTexture;
        
        Texture* _currentTexture = NULL;
    };
    
    RenderDriver* CreateRenderGL2() {
        return new RenderGL2();
    }
}