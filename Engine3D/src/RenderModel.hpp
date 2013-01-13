#pragma once

#include <vector>

#include "OpenGL.hpp"
#include "Shader.hpp"
#include "Vector3f.hpp"
#include "Logger.hpp"

namespace Engine {
	namespace Render {
		enum RenderMode {
			RenderMode_Lines,
			RenderMode_Triangles
		};

		class RenderModel {
			public:
				RenderModel();

				void SetMode(RenderMode mode);
				void SetShader(Shader shader);

				void AddVertex(float x, float y, float z);

				void Draw();
				void Upload();
				void BindShader();
			private:
				std::vector<Vector3f> _vertexs;
				int _vertexCount;

				GLuint _bufferPointer;
				bool _uploaded;

				Shader _currentShader;
				RenderMode _currentRenderMode;
		};
	}
}