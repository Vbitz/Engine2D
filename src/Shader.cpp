#include "Shader.hpp"

namespace Engine {
    Shader::Shader(std::string shaderFilename) {
        this->init("shaders/" + shaderFilename + ".vert", "shaders/" + shaderFilename + ".frag");
    }
    
    void Shader::Begin() {
        if (this->checkProgramPointer()) {
            glUseProgram(this->_programPointer);
        }
    }
    
    void Shader::End() {
        glUseProgram(0);
    }
    
    void Shader::BindUniform(std::string token) {
        if (this->checkProgramPointer()) {
            this->Begin();
            this->_uniforms[token] = glGetUniformLocation(this->_programPointer, token.c_str());
            this->End();
        }
    }
    
    void Shader::UploadUniform(std::string token, GLfloat* data, int verts) {
        if (this->checkProgramPointer()) {
            this->Begin();
            glUniform2fv(this->_uniforms[token], verts, data);
            this->End();
        }
    }
    
    void Shader::BindAttrib(std::string token) {
        if (this->checkProgramPointer()) {
            this->Begin();
            GLint arrayPointer = glGetAttribLocation(this->_programPointer, token.c_str());
            glVertexAttribPointer(arrayPointer, 2, GL_FLOAT, true, 0, 0);
            this->_attribs[token] = arrayPointer;
            this->End();
        }
    }
    
    void Shader::UploadAttrib(std::string token, GLfloat* data, int items) {
        if (this->checkProgramPointer()) {
            this->Begin();
            glUniform2fv(this->_attribs[token], items, data);
            this->End();
        }
    }
    
    void Shader::init(std::string vertShaderFilename, std::string fragShaderFilename) {
        const char* vertShader = Filesystem::GetFileContent(vertShaderFilename);
        const char* fragShader = Filesystem::GetFileContent(fragShaderFilename);
        
        while (!this->compile(vertShader, fragShader)) {
            std::cout << "Could not Compile Shader" << std::endl;
            vertShader = Filesystem::GetFileContent(vertShaderFilename);
            fragShader = Filesystem::GetFileContent(fragShaderFilename);
        }
    }
    
    bool Shader::compile(const char* vertSource, const char* fragSource) {
        this->_vertPointer = glCreateShader(GL_VERTEX_SHADER);
        this->_fragPointer = glCreateShader(GL_FRAGMENT_SHADER);
        
        glShaderSource(this->_vertPointer, 1, &vertSource, NULL);
      
        glCompileShader(this->_vertPointer);
        
        GLint status;
        glGetShaderiv(this->_vertPointer, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE) {
            char buffer[512];
            glGetShaderInfoLog(this->_vertPointer, 512, NULL, buffer);
            std::cout << buffer << std::endl;
            glDeleteShader(this->_vertPointer);
            glDeleteShader(this->_fragPointer);
            return false;
        }
        
        glShaderSource(this->_fragPointer, 1, &fragSource, NULL);
        
        glCompileShader(this->_fragPointer);
        
        glGetShaderiv(this->_fragPointer, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE) {
            char buffer[512];
            glGetShaderInfoLog(this->_fragPointer, 512, NULL, buffer);
            std::cout << buffer << std::endl;
            glDeleteShader(this->_vertPointer);
            glDeleteShader(this->_fragPointer);
            return false;
        }
        
        this->_programPointer = glCreateProgram();
        glAttachShader(this->_programPointer, this->_vertPointer);
        glAttachShader(this->_programPointer, this->_fragPointer);
        
        glLinkProgram(this->_programPointer);
        glUseProgram(this->_programPointer);
        return true;
    }
}