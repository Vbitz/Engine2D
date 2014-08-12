/*
   Filename: VertexBuffer.cpp
   Purpose:  VBO manager
 
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
#include "Filesystem.hpp"
#include "Profiler.hpp"

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
    
    VertexBuffer::VertexBuffer() : _shaderBound(false) {
        
    }
    
    VertexBuffer::VertexBuffer(RenderDriverPtr render, EffectParametersPtr params) : _currentEffect(params), _shaderBound(false), _renderGL(render) {
        this->_init();
    }
    
    VertexBuffer::~VertexBuffer() {
        this->_shutdown();
    }
    
    bool VertexBuffer::IsValid() {
        if (this->_currentEffect == NULL) return false;
        if (this->_renderGL == NULL) return false;
        return true;
    }
    
    void VertexBuffer::Init(RenderDriverPtr render, EffectParametersPtr params) {
        this->_currentEffect = params;
        this->_shaderBound = false;
        this->_renderGL = render;
        this->_init();
    }
    
    void VertexBuffer::_init() {
        this->_renderGL->CheckError("GL3Buffer::_init::Pre");
        if (this->_renderGL->GetOpenGLVersion().major >= 3) {
            glGenVertexArrays(1, &this->_vertexArrayPointer);
        }
        glGenBuffers(1, &this->_elementBufferPointer);
        glGenBuffers(1, &this->_vertexBufferPointer);
        this->_renderGL->CheckError("GL3Buffer::_init::Post");
    }
    
    void VertexBuffer::_shutdown() {
        this->_renderGL->CheckError("GL3Buffer::_shutdown::Pre");
        glDeleteBuffers(1, &this->_vertexBufferPointer);
        glDeleteBuffers(1, &this->_elementBufferPointer);
        if (this->_renderGL->GetOpenGLVersion().major >= 3) {
            glDeleteVertexArrays(1, &this->_vertexArrayPointer);
        }
        this->_renderGL->CheckError("GL3Buffer::_shutdown::Post");
    }
    
    bool VertexBuffer::NeedsUpdate() {
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
    
    bool VertexBuffer::Update() {
        assert(this->_renderGL != NULL);
        if (!this->NeedsUpdate()) {
            return false;
        }
        ENGINE_PROFILER_SCOPE;
        
        this->_renderGL->CheckError("GL3Buffer::Update::Pre");
        
        if (glIsBuffer(this->_vertexBufferPointer)) {
            glDeleteBuffers(1, &this->_vertexBufferPointer);
        }
        
        if (this->_renderGL->GetOpenGLVersion().major >= 3) {
            if (glIsVertexArray(this->_vertexArrayPointer)) {
                glDeleteBuffers(1, &this->_vertexArrayPointer);
            }
        }
        
        this->Invalidate();
        
        this->_init();
        
        this->_renderGL->CheckError("GL3Buffer::Update::PostInit");
        
        this->_getShader()->Update();
        
        this->_renderGL->CheckError("GL3Buffer::Update::PostUpdateShader");
        
        return true;
    }

    void VertexBuffer::AddVert(glm::vec3 pos) {
        this->AddVert(pos, Color4f("white"), glm::vec2(0, 0));
    }
    
    void VertexBuffer::AddVert(glm::vec3 pos, Color4f col) {
        this->AddVert(pos, col, glm::vec2(0, 0));
    }
    
    void VertexBuffer::AddVert(glm::vec3 pos, Color4f col, glm::vec2 uv) {
        this->_vertexBuffer.push_back(BufferFormat({
            .pos = pos,
            .col = col,
            .uv = uv
        }));
        this->_indexBuffer.push_back(this->_vertexCount);
        this->_vertexCount++;
        this->_dirty = true;
    }
    
    void VertexBuffer::Reset() {
        this->_vertexCount = 0;
        this->_vertexBuffer.clear();
        this->_indexBuffer.clear();
        this->_dirty = true;
    }
    
    void VertexBuffer::_upload() {
        ENGINE_PROFILER_SCOPE;
        
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
    
    void VertexBuffer::Draw(PolygonMode mode, glm::mat4 model, glm::mat4 view) {
        if (this->_vertexCount == 0) {
            return; // nothing to draw
        }
        
        ENGINE_PROFILER_SCOPE;
        
        this->_begin();
        
        if (this->_dirty) {
            this->_upload();
            this->_dirty = false;
        }
        
        this->_getShader()->Begin();
        
        this->_getRender()->CheckError("GL3Buffer::Draw::PostBindShader");
        
        GLint viewpoint[4];
        
        glGetIntegerv(GL_VIEWPORT, viewpoint);
        
        glm::mat4 proj = glm::ortho(0.0f, (float) viewpoint[2], (float) viewpoint[3], 0.0f, 1000.0f, -1000.0f);
        
        ShaderSettings settings = this->_currentEffect->GetShaderSettings();
        
        this->_getShader()->UploadUniform(settings.modelMatrixParam, model);
        this->_getShader()->UploadUniform(settings.viewMatrixParam, view);
        this->_getShader()->UploadUniform(settings.projectionMatrixParam, proj);
        
        this->_getRender()->CheckError("VertexBuffer::Draw::PostUploadUniform");
        
        glDrawElements(_polygonModeToGLMode(mode), this->_vertexCount, GL_UNSIGNED_SHORT, 0);
        
        this->_getRender()->CheckError("VertexBuffer::Draw::PostDraw");
        
        this->_getShader()->End();
        
        this->_end();
    }
    
    void VertexBuffer::Save(std::string filename) {
        unsigned char* buff = NULL;
        long fileLength = 0;
        
        VertexBufferDiskFormat header;
        
        fileLength += sizeof(VertexBufferDiskFormat);
        
        header.vertexOffset = fileLength;
        header.vertexCount = this->_vertexBuffer.size();
        
        fileLength += this->_vertexBuffer.size() * sizeof(BufferFormat);
        
        header.indexOffset = fileLength;
        header.indexCount = this->_indexBuffer.size();
        
        fileLength += this->_indexBuffer.size() * sizeof(ushort);
        
        buff = (unsigned char*) std::malloc(fileLength);
        
        std::memcpy(&buff[header.vertexOffset], &this->_vertexBuffer[0],
                    this->_vertexBuffer.size() * sizeof(BufferFormat));
        std::memcpy(&buff[header.indexOffset], &this->_indexBuffer[0],
                    this->_indexBuffer.size() * sizeof(ushort));
        std::memcpy(&buff[0], &header, sizeof(VertexBufferDiskFormat));
        
        Filesystem::WriteFile(filename, (const char*) buff, fileLength);
        
        std::free(buff);
    }
    
    void VertexBuffer::Load(std::string filename) {
        this->Reset();
        
        long fileLength;
        unsigned char* buff = (unsigned char*) Filesystem::GetFileContent(filename, fileLength);
        
        VertexBufferDiskFormat* header = (VertexBufferDiskFormat*) buff;
        
        assert(header->magic[0] == 'E' &&
               header->magic[1] == 'G' &&
               header->magic[2] == 'L' &&
               header->magic[3] == 'B');
        
        this->_vertexBuffer.insert(this->_vertexBuffer.end(),
            (BufferFormat*) &buff[header->vertexOffset],
            (BufferFormat*) &buff[header->vertexOffset + (header->vertexCount * sizeof(BufferFormat))]);
        
        this->_indexBuffer.insert(this->_indexBuffer.end(),
            (ushort*) &buff[header->indexOffset],
            (ushort*) &buff[header->indexOffset + (header->indexCount * sizeof(ushort))]);
        
        this->_vertexCount = header->indexCount;
    }
    
    void VertexBuffer::_begin() {
        if (this->_renderGL->GetOpenGLVersion().major >= 3) {
            glBindVertexArray(this->_vertexArrayPointer);
        }
        glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBufferPointer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_elementBufferPointer);
    }
    
    void VertexBuffer::_end() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        if (this->_renderGL->GetOpenGLVersion().major >= 3) {
            glBindVertexArray(0);
        }
    }
    
    Shader* VertexBuffer::_getShader() {
        if (this->_currentShader == NULL) {
            this->_currentShader = this->_currentEffect->CreateShader();
        }
        return this->_currentShader;
    }
    
    void VertexBuffer::bindShader() {
        ENGINE_PROFILER_SCOPE;
        
        this->_getShader()->Begin();
        
        this->_getRender()->CheckError("VertexBuffer::Upload::PostBeginShader");
        
        ShaderSettings settings = this->_currentEffect->GetShaderSettings();
        
        this->_getShader()->BindUniform(settings.modelMatrixParam);
        this->_getShader()->BindUniform(settings.viewMatrixParam);
        this->_getShader()->BindUniform(settings.projectionMatrixParam);
        
        this->_getRender()->CheckError("VertexBuffer::Upload::PostBindViewpointSize");
        
        this->_getShader()->BindVertexAttrib(settings.vertexParam, 3, 9, 0);
        this->_getShader()->BindVertexAttrib(settings.colorParam, 4, 9, 3);
        this->_getShader()->BindVertexAttrib(settings.texCoardParam, 2, 9, 7);
        
        this->_getRender()->CheckError("VertexBuffer::Upload::PostBindVertexAttributes");
        
        this->_getShader()->End();
    }
}