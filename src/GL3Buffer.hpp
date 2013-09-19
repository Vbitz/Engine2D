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

	private:
		GLuint _vertexArrayPointer, _vertexBufferPointer;
        
        Shader _currentShader;
	};
}