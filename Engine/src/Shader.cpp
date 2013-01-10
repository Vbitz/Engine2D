#include "Shader.hpp"

std::string getShaderPath(Engine::Render::PreDefinedShader shader) {
	switch (shader) {
		case Engine::Render::PreDefinedShader_Basic:
			return "basic";
			break;
		default:
			return "";
			break;
	}
}

namespace Engine {
	namespace Render {
		Shader::Shader() {

		}

		Shader::Shader(PreDefinedShader shader) {
			std::string baseShader = "shaders/" + getShaderPath(shader);
			Logger::WriteVerbose("Loading Shader: " + baseShader + ".frag");
			Logger::WriteVerbose("Loading Shader: " + baseShader + ".vert");
			if (Filesystem::FileExists(baseShader + ".frag")) {
				_init(Filesystem::GetFileContentString(baseShader + ".vert"),
				 Filesystem::GetFileContentString(baseShader + ".frag"));
			} else {
				Logger::WriteError("Could not find Shader");
			}
		}

		Shader::Shader(std::string vertSource, std::string fragSource) {
			_init(vertSource, fragSource);
		}

		void Shader::Activate() {
			if (_checkProgramPointer()) {
				glUseProgram(_programPointer);
			}
		}

		void Shader::BindUniform(std::string id) {
			if (_checkProgramPointer()) {
				this->_uniforms[id] = glGetUniformLocation(_programPointer, id.c_str());
			}
		}

		void Shader::UploadUniform(std::string id, std::vector<Vector3f> vectorData) {
			if (_checkProgramPointer()) {
				int vDataSize = vectorData.size();
				float* vectors = new float[vDataSize * 3];
				for (int i = 0; i < vDataSize; i++) {
					vectors[i * 3] = vectorData[i].X;
					vectors[i * 3 + 1] = vectorData[i].Y;
					vectors[i * 3 + 2] = vectorData[i].Z;
				}
				glUniform3fv(this->_uniforms[id], vDataSize, vectors);
			}
		}

		void Shader::BindAttrib(std::string id) {
			if (_checkProgramPointer()) {
				this->_attribs[id] = glGetAttribLocation(_programPointer, id.c_str());
			}
		}

		bool Shader::_checkProgramPointer() {
			if (_programPointer == 0) {
				Logger::WriteError("Shader is not loaded");
			}
			return _programPointer != 0;
		}

		void Shader::_compile() {

		}

		void Shader::_init(std::string vertSource, std::string fragSource) {
			_programPointer = 0;
			_vertPointer = 0;
			_fragPointer = 0;

			_vertSource = vertSource;
			_fragSource = fragSource;

			_compile();
		}


	}
}