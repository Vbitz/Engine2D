/*
   Filename: GL3Buffer.hpp
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

#pragma once

#include "Shader.hpp"
#include "EffectParameters.hpp"
#include "RenderDriver.hpp"

namespace Engine {
    class Shader;
    class RenderGL3;
    
	class GL3Buffer {
	public:
		GL3Buffer(RenderDriver* render, EffectParameters* params);
		~GL3Buffer();

		void Upload(float* buffer, int count);
		void Draw(GLenum mode, glm::mat4 model, glm::mat4 view, int vertexCount);
        
        bool NeedsUpdate();
        bool Update();
        
        void Invalidate() {
            this->_shaderBound = false;
        }

	private:
        void _init();
        void _shutdown();
        Shader* _getShader();
        
		void bindShader();
        
        RenderDriver* _getRender();
        
        GLuint _vertexArrayPointer, _vertexBufferPointer;
        
        RenderDriver* _renderGL = NULL;
        Shader* _currentShader = NULL;
        EffectParameters* _currentEffect;
        
        bool _shaderBound = false;
	};
}