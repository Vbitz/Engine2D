#pragma once

#include <string>
#include <vector>
#include <map>

#include "OpenGL.hpp"
#include "Logger.hpp"
#include "Vector3f.hpp"
#include "Filesystem.hpp"

namespace Engine {
	namespace Render {
		enum PreDefinedShader {
			PreDefinedShader_Basic
		};

		class Shader {
			public:
				Shader();
				Shader(PreDefinedShader shader);
				Shader(std::string vertSource, std::string fragSource);

				void Activate();

				void BindUniform(std::string id);
				void UploadUniform(std::string id, std::vector<Vector3f> vectorData);

				void BindAttrib(std::string id);
			private:
				bool _checkProgramPointer();

				void _compile();
				void _init(std::string vertSource, std::string fragSource);

				GLuint _programPointer, _vertPointer, _fragPointer;

				std::string _vertSource, _fragSource;
				
				std::map<std::string, GLint> _uniforms;
		};
	}
}