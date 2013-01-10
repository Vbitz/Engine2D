#include "RenderModel.hpp"

GLint getGLMode(Engine::Render::RenderMode mode) {
	switch (mode) {
		case Engine::Render::RenderMode_Lines:
			return GL_LINES;
		case Engine::Render::RenderMode_Triangles:
			return GL_TRIANGLES;
		default:
			return GL_POINTS;
	}
}

namespace Engine {
	namespace Render {
		RenderModel::RenderModel() :  _uploaded(false) {
			this->_bufferPointer = 0;
		}

		void RenderModel::SetMode(RenderMode mode) {
			this->_currentRenderMode = mode;
		}

		void RenderModel::SetShader(Shader shader) {
			this->_currentShader = shader;
		}

		void RenderModel::AddVertex(float x, float y, float z) {
			this->_vertexs.push_back(Vector3f(x, y, z));
			_uploaded = false;
		}

		void RenderModel::Draw() {
			if (!_uploaded) {
				Logger::WriteWarning("Model data is not uploaded, it will be uploaded now, this is a preformance concirn");
				this->Upload();
			}
		}

		void RenderModel::Upload() {
			GLuint bufferPointer;

			glGenBuffers(1, &bufferPointer);

			glBindBuffer(GL_ARRAY_BUFFER, _bufferPointer);

			int vDataSize = _vertexs.size();
			float* vectors = new float[vDataSize * 3];
			for (int i = 0; i < vDataSize; i++) {
				vectors[i * 3] = _vertexs[i].X;
				vectors[i * 3 + 1] = _vertexs[i].Y;
				vectors[i * 3 + 2] = _vertexs[i].Z;
			}

			glBufferData(GL_ARRAY_BUFFER, vDataSize, vectors, GL_STATIC_DRAW);

			_uploaded = true;
			_bufferPointer = bufferPointer;
		}
	}
}