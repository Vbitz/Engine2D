#pragma once

#include "OpenGL.hpp"
#include "Shader.hpp"

namespace Engine {
	namespace Render {
		enum RenderMode {
			RenderMode_Lines,
			RenderMode_Triangles
		};

		class RenderModel {
			public:
				RenderModel();

				void SetMode();
				void AddVertex(float x, float y, float z);

				void Draw();
			private:
				GLint bufferPointer;

				Shader _currentShader;
		};
	}
}