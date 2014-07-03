/*
   Filename: GL3Buffer.cpp
   Purpose:  VBO manager for OpenGL 3.x

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

#include "GL3Buffer.hpp"

#include "Logger.hpp"

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
    
    GL3Buffer::GL3Buffer(RenderDriverPtr render, EffectParametersPtr params) : _currentEffect(params), _shaderBound(false), _renderGL(render) {
        this->_init();
    }
    
    GL3Buffer::~GL3Buffer() {
        this->_shutdown();
    }
    
    void GL3Buffer::_init() {
        this->_renderGL->CheckError("GL3Buffer::_init::Pre");
        glGenVertexArrays(1, &this->_vertexArrayPointer);
        glGenBuffers(1, &this->_elementBufferPointer);
        glGenBuffers(1, &this->_vertexBufferPointer);
        this->_renderGL->CheckError("GL3Buffer::_init::Post");
    }
    
    void GL3Buffer::_shutdown() {
        this->_renderGL->CheckError("GL3Buffer::_shutdown::Pre");
        glDeleteBuffers(1, &this->_vertexBufferPointer);
        glDeleteBuffers(1, &this->_elementBufferPointer);
        glDeleteVertexArrays(1, &this->_vertexArrayPointer);
        this->_renderGL->CheckError("GL3Buffer::_shutdown::Post");
    }
    
    bool GL3Buffer::NeedsUpdate() {
        if (this->_getShader()->NeedsUpdate()) {
            Logger::begin("GL3Buffer", Logger::LogLevel_Verbose) << "GL3Buffer reloaded due to Shader" << Logger::end();
            return true;
        }
        
        if (!glIsBuffer(this->_vertexBufferPointer)) {
            Logger::begin("GL3Buffer", Logger::LogLevel_Verbose) << "GL3Buffer reloaded due to Vertex Buffer" << Logger::end();
            return true;
        }
        
        return false;
    }
    
    bool GL3Buffer::Update() {
        if (!this->NeedsUpdate()) {
            return false;
        }
        RenderDriver::DrawProfiler p = this->_renderGL->Profile(__PRETTY_FUNCTION__);
        
        this->_renderGL->CheckError("GL3Buffer::Update::Pre");
        
        if (glIsBuffer(this->_vertexBufferPointer)) {
            glDeleteBuffers(1, &this->_vertexBufferPointer);
        }
        
        if (glIsVertexArray(this->_vertexArrayPointer)) {
            glDeleteBuffers(1, &this->_vertexArrayPointer);
        }
        
        this->Invalidate();
        
        this->_init();
        
        this->_renderGL->CheckError("GL3Buffer::Update::PostInit");
        
        this->_getShader()->Update();
        
        this->_renderGL->CheckError("GL3Buffer::Update::PostUpdateShader");
        
        return true;
    }

    void GL3Buffer::AddVert(glm::vec3 pos) {
        this->AddVert(pos, Color4f("white"), glm::vec2(0, 0));
    }
    
    void GL3Buffer::AddVert(glm::vec3 pos, Color4f col) {
        this->AddVert(pos, col, glm::vec2(0, 0));
    }
    
    void GL3Buffer::AddVert(glm::vec3 pos, Color4f col, glm::vec2 uv) {
        this->_vertexBuffer.push_back(BufferFormat({
            .pos = pos,
            .col = col,
            .uv = uv
        }));
        this->_indexBuffer.push_back(this->_vertexCount);
        this->_vertexCount++;
        this->_dirty = true;
    }
    
    void GL3Buffer::Reset() {
        this->_vertexCount = 0;
        this->_vertexBuffer.clear();
        this->_indexBuffer.clear();
        this->_dirty = true;
    }
    
    void GL3Buffer::_upload() {
        RenderDriver::DrawProfiler p = this->_renderGL->Profile(__PRETTY_FUNCTION__);
        
        this->_getRender()->CheckError("GL3Buffer::Upload::PreUploadBufferData");
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(BufferFormat) * this->_vertexBuffer.size(), &this->_vertexBuffer[0], GL_STATIC_DRAW);
        
        this->_getRender()->CheckError("GL3Buffer::Upload::PostUploadBufferData");
        
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ushort) * this->_vertexCount, &this->_indexBuffer[0], GL_STATIC_DRAW);
        
        this->_getRender()->CheckError("GL3Buffer::Upload::PostUploadIndexData");
        
        if (!this->_shaderBound) {
            this->bindShader();
            this->_shaderBound = true;
        }
        
        this->_getRender()->CheckError("GL3Buffer::Upload::Post");
    }
    
    void GL3Buffer::Draw(PolygonMode mode, glm::mat4 model, glm::mat4 view) {
        if (this->_vertexCount == 0) {
            return; // nothing to draw
        }
        
        RenderDriver::DrawProfiler p = this->_renderGL->Profile(__PRETTY_FUNCTION__);
        
        this->_begin();
        
        if (this->_dirty) {
            this->_upload();
            this->_dirty = false;
        }
        
        this->_getShader()->Begin();
        
        this->_getRender()->CheckError("GL3Buffer::Draw::PostBindShader");
        
        GLint viewpoint[4];
        
        glGetIntegerv(GL_VIEWPORT, viewpoint);
        
        glm::mat4 proj = glm::ortho(0.0f, (float) viewpoint[2], (float) viewpoint[3], 0.0f, 1.0f, -1.0f);
        
        ShaderSettings settings = this->_currentEffect->GetShaderSettings();
        
        this->_getShader()->UploadUniform(settings.modelMatrixParam, model);
        this->_getShader()->UploadUniform(settings.viewMatrixParam, view);
        this->_getShader()->UploadUniform(settings.projectionMatrixParam, proj);
        
        this->_getRender()->CheckError("GL3Buffer::Draw::PostUploadUniform");
        
        glDrawElements(_polygonModeToGLMode(mode), this->_vertexCount, GL_UNSIGNED_SHORT, 0);
        
        this->_getRender()->CheckError("GL3Buffer::Draw::PostDraw");
        
        this->_getShader()->End();
        
        this->_end();
    }
    
    void GL3Buffer::_begin() {
        glBindVertexArray(this->_vertexArrayPointer);
        glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBufferPointer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_elementBufferPointer);
    }
    
    void GL3Buffer::_end() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
    Shader* GL3Buffer::_getShader() {
        if (this->_currentShader == NULL) {
            this->_currentShader = this->_currentEffect->CreateShader();
        }
        return this->_currentShader;
    }
    
    void GL3Buffer::bindShader() {
        RenderDriver::DrawProfiler p = this->_renderGL->Profile(__PRETTY_FUNCTION__);
        
        this->_getShader()->Begin();
        
        this->_getRender()->CheckError("GL3Buffer::Upload::PostBeginShader");
        
        ShaderSettings settings = this->_currentEffect->GetShaderSettings();
        
        this->_getShader()->BindUniform(settings.modelMatrixParam);
        this->_getShader()->BindUniform(settings.viewMatrixParam);
        this->_getShader()->BindUniform(settings.projectionMatrixParam);
        
        this->_getRender()->CheckError("GL3Buffer::Upload::PostBindViewpointSize");
        
        this->_getShader()->BindVertexAttrib(settings.vertexParam, 3, 9, 0);
        this->_getShader()->BindVertexAttrib(settings.colorParam, 4, 9, 3);
        this->_getShader()->BindVertexAttrib(settings.texCoardParam, 2, 9, 7);
        
        this->_getRender()->CheckError("GL3Buffer::Upload::PostBindVertexAttributes");
        
        this->_getShader()->End();
    }
}