#pragma once

#include "common.hpp"
#include "Filesystem.hpp"

#include <vector>
#include <map>

namespace Engine {
    class Shader {
    public:
        Shader();
        Shader(std::string shaderFilename);
        
        ~Shader();
        
        void Init(std::string vertShaderFilename, std::string fragShaderFilename);
        
        void Begin();
        void End();
        
        bool Update();
        bool NeedsUpdate();
        
        void BindUniform(std::string token);
        void UploadUniform(std::string token, GLfloat* data, int verts);
        void UploadUniform(std::string token, glm::mat4 matrix);
        void UploadUniform(std::string token, GLfloat x, GLfloat y);
        
        void BindVertexAttrib(std::string token, int attribSize, int totalSize, int stride);
        
        bool IsLoaded() {
            return this->_loaded;
        }
        
    private:
        bool compile(const char* vertSource, const char* fragSource);
        
        bool checkProgramPointer();
        
        bool _loaded;
        
        std::map<std::string, GLuint> _uniforms;
        std::map<std::string, GLuint> _attribs;
        
        GLuint _programPointer, _vertPointer, _fragPointer;
        std::string _vertFilename, _fragFilename;
        unsigned long _vertLastModify, _fragLastModify;
    };
}