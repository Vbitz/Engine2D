#pragma once

#include "common.hpp"
#include "Filesystem.hpp"

#include <vector>
#include <map>

namespace Engine {
    class Shader {
    public:
        Shader(std::string shaderFilename);
        
        void Begin();
        void End();
        
        void BindUniform(std::string token);
        void UploadUniform(std::string token, GLfloat* data, int verts);
        
        void BindAttrib(std::string token);
        void UploadAttrib(std::string token, GLfloat* data, int items);
        
        
    private:
        void init(std::string vertShaderFilename, std::string fragShaderFilename);
        bool compile(std::string vertShader, std::string fragShader);
        
        inline bool checkProgramPointer() {
            return this->_programPointer != 0;
        }
        
        std::map<std::string, GLuint> _uniforms;
        std::map<std::string, GLuint> _attribs;
        
        GLuint _programPointer, _vertPointer, _fragPointer;
    };
}