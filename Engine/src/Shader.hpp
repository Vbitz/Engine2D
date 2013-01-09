#pragma once

#include <string>

#include "OpenGL.hpp"

namespace Engine {
	namespace Render {
		enum PreDefinedShader {
			PreDefinedShader_Basic
		};

		class Shader {
			public:
				Shader(PreDefinedShader shader);
				Shader(std::string vertSource, std::string fragSource);
		};
	}
}