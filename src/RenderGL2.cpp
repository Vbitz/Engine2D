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
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

namespace Engine {
    inline GLenum _polygonModeToGLMode(PolygonMode mode) {
        switch (mode) {
            case PolygonMode::Invalid: return NULL;
                
            case PolygonMode::Triangles: return GL_TRIANGLES;
            case PolygonMode::TriangleFan: return GL_TRIANGLE_FAN;
            case PolygonMode::TriangleStrip: return GL_TRIANGLE_STRIP;
                
            case PolygonMode::Lines: return GL_LINES;
            case PolygonMode::LineLoop: return GL_LINE_LOOP;
            case PolygonMode::LineStrip: return GL_LINE_STRIP;
                
            default: return NULL;
        }
    }
    
    class RenderGL2 : public RenderDriver {
    public:
        RendererType GetRendererType() override {
            return RendererType::OpenGL2;
        }
        
        OpenGLVersion GetOpenGLVersion() override {
            return GetAppSingilton()->GetWindow()->GetGlVersion();
        }
        
        EffectShaderType GetBestEffectShaderType() override {
            return EffectShaderType::GLSL_150;
        }
        
        bool CheckError(const char* source) override {
            GLenum err;
            bool oneError = false;
            while ((err = glGetError()) != GL_NO_ERROR) {
                Logger::begin("OpenGL", Logger::LogLevel_Error) << "GLError in " << source << " : " << GLErrorString(err) << Logger::end();
                Platform::DumpStackTrace();
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
            RenderDriver::DrawProfiler p = this->Profile(__PRETTY_FUNCTION__);
            
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
            RenderDriver::DrawProfiler p = this->Profile(__PRETTY_FUNCTION__);
            
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
        
        void BeginRendering(PolygonMode mode) override {
            glBegin(_polygonModeToGLMode(mode));
        }
        
        void EndRendering() override {
            glEnd();
        }
        
        void EnableTexture(Texture* texId) override {
            RenderDriver::DrawProfiler p = this->Profile(__PRETTY_FUNCTION__);
            
            glEnable(GL_TEXTURE_2D);
                
            texId->Begin();
        }
        
        void DisableTexture() override {
            RenderDriver::DrawProfiler p = this->Profile(__PRETTY_FUNCTION__);
            
            glBindTexture(GL_TEXTURE_2D, 0);
                
            glDisable(GL_TEXTURE_2D);
        }
        
        void EnableSmooth() override {
            RenderDriver::DrawProfiler p = this->Profile(__PRETTY_FUNCTION__);
            
            glEnable(GL_LINE_SMOOTH);
            glEnable(GL_POLYGON_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
        }
        
        void DisableSmooth() override {
            RenderDriver::DrawProfiler p = this->Profile(__PRETTY_FUNCTION__);
            
            glDisable(GL_LINE_SMOOTH);
            glDisable(GL_POLYGON_SMOOTH);
        }
        
        void FlushAll() override { }
		
        void Init2d() override { }
        
		void Begin2d() override {
            RenderDriver::DrawProfiler p = this->Profile(__PRETTY_FUNCTION__);
            
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
    };
    
    RenderDriver* CreateRenderGL2() {
        return new RenderGL2();
    }
}