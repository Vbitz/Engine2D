/*
   Filename: VertexBuffer.hpp
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

#pragma once

#include "Shader.hpp"
#include "RenderDriver.hpp"

namespace Engine {
    class Shader;
    class RenderGL3;
    
    /*
     Buffer format
     (x, y, z)      Position
     (r, g, b, a)   Color
     (u, v)         TexCourd
     */
    
#pragma pack(push, 1)
    struct BufferFormat {
		BufferFormat(glm::vec3 pos, Color4f col, glm::vec2 uv) : pos(pos), col(col), uv(uv) {}

        glm::vec3 pos;
        Color4f col;
        glm::vec2 uv;
    };
#pragma pack(pop)
    
    // .eglb format
    struct VertexBufferDiskFormat {
        unsigned char magic[4];
        
        unsigned int vertexOffset;
        unsigned int vertexCount;
        
        unsigned int indexOffset;
        unsigned int indexCount;
    };
    
    typedef std::vector<BufferFormat> VertexStore;
    typedef std::vector<unsigned short> IndexStore;
    
    typedef VertexStore& VertexStoreRef;
    typedef IndexStore& IndexStoreRef;
    
    // Very temporory until I have a better API to do it.
    struct Camera {
        
    };
    
    ENGINE_CLASS(VertexBuffer);
    
	class VertexBuffer {
	public:
        enum class ProjectionType {
            Orthographic,
            Perspective
        };
        
        VertexBuffer();
		VertexBuffer(RenderDriverPtr render, EffectParametersPtr params);
		~VertexBuffer();

        bool IsValid();
        void Init(RenderDriverPtr render, EffectParametersPtr params);

        void AddVert(glm::vec3 pos);
        void AddVert(glm::vec3 pos, Color4f col);
        void AddVert(glm::vec3 pos, Color4f col, glm::vec2 uv);
        
        void Reset();
        
		void Draw(PolygonMode mode, glm::mat4 model);
        
        bool NeedsUpdate();
        bool Update();
        
        void Invalidate() {
            this->_shaderBound = false;
        }
        
        void Save(std::string filename);
        void Load(std::string filename);

        void SetDepthTest(bool depthTest) {
            this->_depthTest = depthTest;
        }
        void SetWireframe(bool wireframe) {
            this->_wireframe = wireframe;
        }
        void SetProjectionType(ProjectionType t);
        void SetLookAtView(glm::vec3 source, glm::vec3 target);
        
        void ComputeNormals(PolygonMode polygonFormat);
        
        RenderDriverPtr GetRender() {
            return this->_renderGL;
        }
        
	private:
        void _init();
        void _shutdown();
        ShaderPtr _getShader();
        
        void _begin();
        void _end();
        
		void bindShader();
        
		void _upload();
        
        glm::vec4 _getCameraView();
        
        unsigned int _vertexArrayPointer, _elementBufferPointer, _vertexBufferPointer;
        
        ProjectionType _projectionType = ProjectionType::Orthographic;
        
        glm::mat4 _view;
        
        VertexStore _vertexBuffer;
        IndexStore _indexBuffer;
        
        unsigned short _vertexCount = 0;
        
        bool _dirty = false;
        bool _depthTest = false;
        bool _wireframe = false;
        
        RenderDriverPtr _renderGL = NULL;
        ShaderPtr _currentShader = NULL;
        EffectParametersPtr _currentEffect = NULL;
        
        bool _shaderBound = false;
	};
}