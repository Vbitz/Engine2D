#include "Shader.hpp"

#include "Draw2D.hpp"

namespace Engine {
    Shader::Shader() : _loaded(false) {
        
    }
    
    Shader::Shader(std::string shaderFilename) : _loaded(false) {
        this->_vertFilename = "shaders/" + shaderFilename + ".vert";
        this->_fragFilename = "shaders/" + shaderFilename + ".frag";
        this->Init(this->_vertFilename, this->_fragFilename);
    }
    
    Shader::~Shader() {
        if (!this->_loaded) {
            this->End();
            glDeleteShader(this->_vertPointer);
            glDeleteShader(this->_fragPointer);
            glDeleteProgram(this->_programPointer);
        }
    }
    
    void Shader::Begin() {
        if (this->checkProgramPointer()) {
            glUseProgram(this->_programPointer);
        }
    }
    
    void Shader::End() {
        glUseProgram(0);
    }
    
    bool Shader::Update() {
        if (this->NeedsUpdate()) {
            glDeleteShader(this->_vertPointer);
            glDeleteShader(this->_fragPointer);
            glDeleteProgram(this->_programPointer);
            this->_loaded = false;
            this->Init(this->_vertFilename, this->_fragFilename);
            return true;
        } else {
            return false;
        }
    }
    
    bool Shader::NeedsUpdate() {
        static float lastUpdate = Logger::GetTime();
        
        if ((Logger::GetTime() - lastUpdate) > 0.2) {
            lastUpdate = Logger::GetTime();
            return Filesystem::GetFileModifyTime(this->_vertFilename) > this->_vertLastModify
            || Filesystem::GetFileModifyTime(this->_fragFilename) > this->_fragLastModify;
        } else {
            return false;
        }
    }
    
    void Shader::BindUniform(std::string token) {
        if (this->checkProgramPointer()) {
            this->Begin();
            this->_uniforms[token] = glGetUniformLocation(this->_programPointer, token.c_str());
            this->End();
        }
    }
    
    void Shader::UploadUniform(std::string token, GLfloat x, GLfloat y) {
        if (this->checkProgramPointer()) {
            glUniform2f(this->_uniforms[token], x, y);
        }
    }
    
    void Shader::UploadUniform(std::string token, glm::mat4 matrix) {
        if (this->checkProgramPointer()) {
            glUniformMatrix4fv(this->_uniforms[token], 1, GL_FALSE, &matrix[0][0]);
        }
    }
    
    void Shader::UploadUniform(std::string token, GLfloat* data, int verts) {
        if (this->checkProgramPointer()) {
            glUniform2fv(this->_uniforms[token], verts, data);
        }
    }
    
    void Shader::BindVertexAttrib(std::string token, int attribSize, int totalSize, int stride) {
        if (!this->checkProgramPointer()) {
            return;
        }
        
        GLuint attribPos = glGetAttribLocation(_programPointer, token.c_str());
        
        if ((int) attribPos == -1) {
            Logger::begin("Shader", Logger::LogLevel_Error) << "Bad token name: " << token << Logger::end();
        }
        
        Draw2D::CheckGLError("Shader::BindVertexAttrib::PostGetAttribLocation");
        
        glVertexAttribPointer(attribPos, attribSize, GL_FLOAT, GL_FALSE,
                              totalSize * sizeof(float),
                              stride == 0 ? NULL : (void*)(stride * sizeof(float)));
        
        Draw2D::CheckGLError("Shader::BindVertexAttrib::PostVertexAttribPointer");
        
        glEnableVertexAttribArray(attribPos);
        
        Draw2D::CheckGLError("Shader::BindVertexAttrib::PostEnable");
    }
    
    void Shader::Init(std::string vertShaderFilename, std::string fragShaderFilename) {
        this->_vertFilename = vertShaderFilename;
        this->_fragFilename = fragShaderFilename;
        const char* vertShader = Filesystem::GetFileContent(vertShaderFilename);
        const char* fragShader = Filesystem::GetFileContent(fragShaderFilename);
        
        this->_vertLastModify = Filesystem::GetFileModifyTime(vertShaderFilename);
        this->_fragLastModify = Filesystem::GetFileModifyTime(fragShaderFilename);
        
        bool firstTime = true;
        
        while (!this->compile(vertShader, fragShader)) {
            if (firstTime) {
                Logger::begin("Shader", Logger::LogLevel_Error) << "Could not Compile Shader" << Logger::end();
                firstTime = false;
            }
            vertShader = Filesystem::GetFileContent(vertShaderFilename);
            fragShader = Filesystem::GetFileContent(fragShaderFilename);
        }
        this->_loaded = true;
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
            Logger::begin("Shader", Logger::LogLevel_Error) << buffer << Logger::end();
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
            Logger::begin("Shader", Logger::LogLevel_Error) << buffer << Logger::end();
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