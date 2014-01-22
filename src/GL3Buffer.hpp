#pragma once

#include "common.hpp"

#include "Shader.hpp"
#include "EffectParameters.hpp"

namespace Engine {
	class GL3Buffer {
	public:
		GL3Buffer(EffectParameters params);
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
        
        GLuint _vertexArrayPointer, _vertexBufferPointer;
        
        Shader* _currentShader = NULL;
        EffectParameters _currentEffect;
        
        bool _shaderBound = false;
	};
}