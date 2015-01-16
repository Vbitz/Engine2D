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

#include <cstring>

#include "Logger.hpp"
#include "Filesystem.hpp"
#include "Profiler.hpp"
#include "Application.hpp"
#include "Config.hpp"

#define GLM_FORCE_RADIANS
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

namespace Engine {
    inline GLenum _polygonModeToGLMode(PolygonMode mode) {
        switch (mode) {
            case PolygonMode::Invalid: return (GLenum) 0;
                
            case PolygonMode::Triangles: return GL_TRIANGLES;
            case PolygonMode::TriangleFan: return GL_TRIANGLE_FAN;
            case PolygonMode::TriangleStrip: return GL_TRIANGLE_STRIP;
                
            case PolygonMode::Lines: return GL_LINES;
            case PolygonMode::LineLoop: return GL_LINE_LOOP;
            case PolygonMode::LineStrip: return GL_LINE_STRIP;
                
            default: return (GLenum) 0;
        }
    }
    
	VertexBuffer::VertexBuffer() : _shaderBound(false), _uuid(Platform::GenerateUUID()),
		_vertexBuffer(512, BufferFormat(glm::vec3(), Color4f(0, 0, 0, 1), glm::vec3())) {

    }
    
	VertexBuffer::VertexBuffer(RenderDriverPtr render, EffectParametersPtr params) : _currentEffect(params),
			_shaderBound(false), _renderGL(render), _uuid(Platform::GenerateUUID()),
			_vertexBuffer(512, BufferFormat(glm::vec3(), Color4f(0, 0, 0, 1), glm::vec3())) {
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
		this->_renderGL->CheckError("VertexBuffer::_init::Pre");
		Logger::begin("VertexBuffer", Logger::LogLevel_Verbose) << "VertexBuffer[" << Platform::StringifyUUID(this->_uuid) << "] _init" << Logger::end();
        if (this->_renderGL->GetOpenGLVersion().major >= 3) {
            glGenVertexArrays(1, &this->_vertexArrayPointer);
        }
        glGenBuffers(1, &this->_vertexBufferPointer);
        this->_renderGL->CheckError("VertexBuffer::_init::Post");
    }
    
    void VertexBuffer::_shutdown() {
		this->_renderGL->CheckError("VertexBuffer::_shutdown::Pre");
		Logger::begin("VertexBuffer", Logger::LogLevel_Verbose) << "VertexBuffer[" << Platform::StringifyUUID(this->_uuid) << "] _shutdown" << Logger::end();
        glDeleteBuffers(1, &this->_vertexBufferPointer);
        if (this->_renderGL->GetOpenGLVersion().major >= 3) {
            glDeleteVertexArrays(1, &this->_vertexArrayPointer);
        }
        this->_renderGL->CheckError("VertexBuffer::_shutdown::Post");
    }
    
    bool VertexBuffer::NeedsUpdate() {
        if (this->_getShader()->NeedsUpdate()) {
			Logger::begin("VertexBuffer", Logger::LogLevel_Verbose) << "VertexBuffer[" << Platform::StringifyUUID(this->_uuid) << "] reloaded due to Shader" << Logger::end();
            return true;
        }
        
		if (!glIsBuffer(this->_vertexBufferPointer)) {
			Logger::begin("VertexBuffer", Logger::LogLevel_Verbose) << "VertexBuffer[" << Platform::StringifyUUID(this->_uuid) << "] reloaded due to Buffer" << Logger::end();
            return true;
        }
        
        return false;
    }
    
    bool VertexBuffer::Update() {
        assert(this != NULL);
        assert(this->_renderGL != NULL);
        if (!this->NeedsUpdate()) {
            return false;
        }
        ENGINE_PROFILER_SCOPE;
        
        this->_renderGL->CheckError("VertexBuffer::Update::Pre");
        
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
        
        this->_renderGL->CheckError("VertexBuffer::Update::PostInit");
        
        this->_getShader()->Update();
        
        this->_renderGL->CheckError("VertexBuffer::Update::PostUpdateShader");
        
        return true;
    }
    
    void VertexBuffer::Reset() {
        this->_vertexCount = 0;
        this->_dirty = true;
    }
    
    void VertexBuffer::_upload() {
        ENGINE_PROFILER_SCOPE;
        
        this->GetRender()->CheckError("VertexBuffer::Upload::PreUploadBufferData");
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(BufferFormat) * this->_vertexCount, &this->_vertexBuffer[0], GL_STATIC_DRAW);
        
        this->GetRender()->CheckError("VertexBuffer::Upload::PostUploadBufferData");
        
        if (!this->_shaderBound) {
            this->bindShader();
            this->_shaderBound = true;
        }
        
        this->GetRender()->CheckError("GL3Buffer::Upload::Post");
    }
    
    void VertexBuffer::Draw(PolygonMode mode, glm::mat4 model) {
        
        if (this->_vertexCount == 0) {
            return; // nothing to draw
        }
#ifdef PROFILER
		static char* addedData = NULL;
        if (addedData == NULL) {
            addedData = new char[128];
        }
        
        sprintf(addedData, "_vertexCount=%d", this->_vertexCount);

		ENGINE_PROFILER_SCOPE_EX(addedData);
#endif
        
        this->_renderGL->TrackStat(RenderStatistic::DrawCall, 1);
        this->_renderGL->TrackStat(RenderStatistic::Verts, this->_vertexCount);
        
        this->_begin();
        
        if (this->_dirty) {
            this->_upload();
            this->_dirty = false;
        }
        
        this->_getShader()->Begin();
        
        this->GetRender()->CheckError("VertexBuffer::Draw::PostBindShader");
        
        ShaderSettings settings = this->_currentEffect->GetShaderSettings();
        
        glm::mat4 proj;
        switch (this->_projectionType) {
            case ProjectionType::Orthographic:
                proj = GetAppSingilton()->GetWindow()->GetOrthoProjection();
                break;
            case ProjectionType::Perspective:
                glm::vec2 windowSize = GetAppSingilton()->GetWindow()->GetWindowSize();
                proj = glm::perspective(glm::radians(Config::GetFloat("core.render.fovy")), GetAppSingilton()->GetWindow()->GetAspectRatio(), 1.0f, 1000.0f);
                break;
        }
        
        this->_getShader()->UploadUniform(settings.modelMatrixParam, model);
        this->_getShader()->UploadUniform(settings.viewMatrixParam, this->_view);
        this->_getShader()->UploadUniform(settings.projectionMatrixParam, proj);
        
        this->GetRender()->CheckError("VertexBuffer::Draw::PostUploadUniform");
        
        this->GetRender()->Set(RenderStateFlag::DepthTest, this->_depthTest);
        
        if (this->_wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        
        {
            ENGINE_PROFILER_SCOPE_EX("glDrawElements");
            glDrawArrays(_polygonModeToGLMode(mode), 0, this->_vertexCount);
        }
        
        if (this->_wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        
        this->GetRender()->CheckError("VertexBuffer::Draw::PostDraw");
        
        this->_getShader()->End();
        
        this->_end();
    }
    
    void VertexBuffer::Save(std::string filename) {
        unsigned char* buff = NULL;
        long fileLength = 0;
        
		// Anoying VS
        VertexBufferDiskFormat header;
		header.magic[0] = 'E';
		header.magic[1] = 'G';
		header.magic[2] = 'L';
		header.magic[3] = 'B';
        
        fileLength += sizeof(VertexBufferDiskFormat);
        
        header.vertexOffset = fileLength;
        header.vertexCount = this->_vertexBuffer.size();
        
        fileLength += this->_vertexBuffer.size() * sizeof(BufferFormat);
        
        header.indexOffset = 0;
        header.indexCount = 0;
        
        fileLength += 0 * sizeof(unsigned short);
        
        buff = new unsigned char[fileLength];
        
        std::memcpy(&buff[header.vertexOffset], &this->_vertexBuffer[0],
                    this->_vertexBuffer.size() * sizeof(BufferFormat));
        std::memcpy(&buff[0], &header, sizeof(VertexBufferDiskFormat));
        
        Filesystem::WriteFile(filename, (const char*) buff, fileLength);
        
        delete [] buff;
    }
    
    void VertexBuffer::Load(std::string filename) {
        this->Reset();

		this->_vertexBuffer.clear();
        
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
        
        this->_vertexCount = header->indexCount;
    }
    
    void VertexBuffer::SetProjectionType(ProjectionType t) {
        this->_projectionType = t;
    }
    
    void VertexBuffer::SetLookAtView(glm::vec3 source, glm::vec3 target) {
        this->_view = glm::lookAt(source, target, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    
    // Code based on Three.js which is in turn based on
    // http://www.iquilezles.org/www/articles/normals/normals.htm
    // in part
    void VertexBuffer::ComputeNormals(PolygonMode polygonFormat) {
        ENGINE_PROFILER_SCOPE;
        
        // TODO: Load them into the normal buffer insteed of the color buffer
        if (polygonFormat != PolygonMode::Triangles) return;
        
        glm::vec3 *normals = new glm::vec3[this->_vertexCount];
        
        // calculate normals
        for (int i = 0; i < this->_vertexCount; i += 3) {
            glm::vec3 faceNormal = glm::normalize(glm::cross(
                (this->_vertexBuffer[i + 2].pos - this->_vertexBuffer[i + 1].pos),
                (this->_vertexBuffer[i + 0].pos - this->_vertexBuffer[i + 1].pos)));
            normals[i + 0] += faceNormal;
            normals[i + 1] += faceNormal;
            normals[i + 2] += faceNormal;
        }
        
        // normalize the normals and load them into the buffer
        for (int i = 0; i < this->_vertexCount; i++) {
            normals[i] = glm::normalize(normals[i]);
            this->_vertexBuffer[i].col = Color4f(normals[i].x, normals[i].y, normals[i].z, 1.0f);
        }
        
        delete [] normals;
    }
    
    void VertexBuffer::_begin() {
        if (this->_renderGL->GetOpenGLVersion().major >= 3) {
            glBindVertexArray(this->_vertexArrayPointer);
        }
        glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBufferPointer);
    }
    
    void VertexBuffer::_end() { /* noop */ }
    
    Shader* VertexBuffer::_getShader() {
        if (this->_currentShader == NULL) {
            this->_currentShader = this->_currentEffect->CreateShader();
        }
        return this->_currentShader;
    }
    
    void VertexBuffer::bindShader() {
        ENGINE_PROFILER_SCOPE;
        
        this->_getShader()->Begin();
        
        this->GetRender()->CheckError("VertexBuffer::Upload::PostBeginShader");
        
        ShaderSettings settings = this->_currentEffect->GetShaderSettings();
        
        this->_getShader()->BindUniform(settings.modelMatrixParam);
        this->_getShader()->BindUniform(settings.viewMatrixParam);
        this->_getShader()->BindUniform(settings.projectionMatrixParam);
        
        this->GetRender()->CheckError("VertexBuffer::Upload::PostBindViewpointSize");
        
        this->_getShader()->BindVertexAttrib(settings.vertexParam, 3, 10, 0);
        this->_getShader()->BindVertexAttrib(settings.colorParam, 4, 10, 3);
        this->_getShader()->BindVertexAttrib(settings.texCoardParam, 3, 10, 7);
        
        this->GetRender()->CheckError("VertexBuffer::Upload::PostBindVertexAttributes");
        
        this->_getShader()->End();
    }
}
