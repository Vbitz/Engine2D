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
		Shader* _currentShader = NULL;

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
				_currentShader = this;
			}
		}

		void Shader::BindUniform(std::string id) {
			if (_checkProgramPointer()) {
				this->Activate();
				this->_uniforms[id] = glGetUniformLocation(_programPointer, id.c_str());
			}
		}

		void Shader::UploadUniform(std::string id, std::vector<Vector3f> vectorData) {
			if (_checkProgramPointer()) {
				this->Activate();
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
				this->Activate(); // checks for program pointer twice
				GLint arrayPointer = glGetAttribLocation(_programPointer, id.c_str());
				glVertexAttribPointer(arrayPointer, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(arrayPointer);
			}
		}

		bool Shader::_checkProgramPointer() {
			if (_programPointer == 0) {
				Logger::WriteError("Shader is not loaded");
				return false;
			} else {
				return true;
			}
		}

		void Shader::_compile() {
			_vertPointer = glCreateShader(GL_VERTEX_SHADER);
			_fragPointer = glCreateShader(GL_FRAGMENT_SHADER);

			const char* vertSourceStr = _vertSource.c_str();

			glShaderSource(_vertPointer, 1, &vertSourceStr, NULL);

			glCompileShader(_vertPointer);

			GLint status;
			glGetShaderiv(_vertPointer, GL_COMPILE_STATUS, &status);
			if (status != GL_TRUE) {
				char buffer[512];
				glGetShaderInfoLog(_vertPointer, 512, NULL, buffer);
				Logger::WriteError(std::string(buffer));
			}

			const char* fragSourceStr = _fragSource.c_str();

			glShaderSource(_fragPointer, 1, &fragSourceStr, NULL);

			glCompileShader(_fragPointer);

			glGetShaderiv(_fragPointer, GL_COMPILE_STATUS, &status);
			if (status != GL_TRUE) {
				char buffer[512];
				glGetShaderInfoLog(_fragPointer, 512, NULL, buffer);
				Logger::WriteError(std::string(buffer));
			}

			_programPointer = glCreateProgram();
			glAttachShader(_programPointer, _vertPointer);
			glAttachShader(_programPointer, _fragPointer);

			glLinkProgram(_programPointer);
			glUseProgram(_programPointer);
		}

		void Shader::_init(std::string vertSource, std::string fragSource) {
			_programPointer = 0;
			_vertPointer = 0;
			_fragPointer = 0;

			_vertSource = vertSource;
			_fragSource = fragSource;

			std::cout << "vert: " << vertSource << std::endl << "frag: " << fragSource << std::endl;

			_compile();
		}


	}
}