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
    GL3Buffer::GL3Buffer(RenderDriver* render, EffectParameters* params) : _currentEffect(params), _shaderBound(false), _renderGL(render) {
        this->_init();
    }
    
    GL3Buffer::~GL3Buffer() {
        this->_shutdown();
    }
    
    void GL3Buffer::_init() {
        glGenVertexArrays(1, &this->_vertexArrayPointer);
        glGenBuffers(1, &this->_elementBufferPointer);
        glGenBuffers(1, &this->_vertexBufferPointer);
        this->_renderGL->CheckError("GL3Buffer::_init::Post");
    }
    
    void GL3Buffer::_shutdown() {
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
        
        if (glIsBuffer(this->_vertexBufferPointer)) {
            glDeleteBuffers(1, &this->_vertexBufferPointer);
        }
        
        if (glIsVertexArray(this->_vertexArrayPointer)) {
            glDeleteBuffers(1, &this->_vertexArrayPointer);
        }
        
        this->Invalidate();
        
        this->_init();
        
        this->_getShader()->Update();
        
        return true;
    }
    
    void GL3Buffer::Upload(float *vertBuffer, ushort* indexBuffer, int count, size_t formatSize) {
        RenderDriver::DrawProfiler p = this->_renderGL->Profile(__PRETTY_FUNCTION__);
        
        this->Update();
        
        glBindVertexArray(this->_vertexArrayPointer);
        glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBufferPointer);
        
        this->_getRender()->CheckError("GL3Buffer::Upload::PreUploadBufferData");
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * formatSize * count, vertBuffer, GL_STATIC_DRAW);
        
        this->_getRender()->CheckError("GL3Buffer::Upload::PostUploadBufferData");
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_elementBufferPointer);
        
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ushort) * count, indexBuffer, GL_STATIC_DRAW);
        
        this->_getRender()->CheckError("GL3Buffer::Upload::PostUploadIndexData");
        
        if (!this->_shaderBound) {
            this->bindShader();
            this->_shaderBound = true;
        }
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
    void GL3Buffer::Draw(int mode, glm::mat4 model, glm::mat4 view, int vertexCount) {
        glBindVertexArray(this->_vertexArrayPointer);
        glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBufferPointer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_elementBufferPointer);
        
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
        
        glDrawElements(mode, vertexCount, GL_UNSIGNED_SHORT, 0);
        
        this->_getRender()->CheckError("GL3Buffer::Draw::PostDraw");
        
        this->_getShader()->End();
        
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
    
    RenderDriver* GL3Buffer::_getRender() {
        return this->_renderGL;
    }
}