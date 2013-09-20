#pragma once

#include "common.hpp"

#include "Shader.hpp"

namespace Engine {
	class GL3Buffer {
	public:
		GL3Buffer(Shader shader);
		~GL3Buffer();

		void Upload(float* buffer, int count);
		void Draw(GLenum mode, int vertexCount);
        
        void Invalidate() {
            this->_shaderBound = false;
        }

	private:
		void bindShader();
        
        GLuint _vertexArrayPointer, _vertexBufferPointer;
        
        Shader _currentShader;
        
        bool _shaderBound = false;
	};
}