/*
   Filename: RenderGL3.hpp
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

#pragma once

#include "RenderDriver.hpp"

#include "TextureLoader.hpp"
#include "GL3Buffer.hpp"

#define BUFFER_SIZE 4096

namespace Engine {
    class Texture;
    class GL3Buffer;
    class EffectParameters;
    
    class RenderGL3 : public RenderDriver {
    public:
        RendererType GetRendererType() override;
        OpenGLVersion GetOpenGLVersion() override;
        EffectShaderTypes::Type GetBestEffectShaderType() override;
        
        bool CheckError(const char* source) override;
        
        bool HasExtention(std::string extentionName) override;
        std::vector<std::string> GetExtentions() override;
        
        void ResetMatrix() override;
        
        void BeginRendering(int mode) override;
        
        void EndRendering() override;
        
        void EnableTexture(Texture* texId) override;
        void DisableTexture() override;
        
        void EnableSmooth() override;
        void DisableSmooth() override;
        
        void Print(float x, float y, const char* string) override;
        
        void FlushAll() override;
		
        void Init2d() override;
        
		void Begin2d() override;
		void End2d() override;
        
        void Reset() override;
        
        void SetCenter(float x, float y) override;
        void CameraPan(float x, float y) override;
        void CameraZoom(float f) override;
        void CameraRotate(float r) override;
    protected:
        void _clearColor(Color4f col) override;
        void _addVert(float x, float y, float z, Color4f col, float s, float t) override;
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
}