/*
   Filename: Shader.hpp
   Purpose:  OpenGL shader control and loading

   Part of Engine2D

   Copyright (C) 2014 Vbitz

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#pragma once

#include <vector>
#include <map>

#include "Window.hpp"

#include "RenderDriver.hpp"

namespace Engine {
    class RenderGL3;
    
    class Shader {
    public:
        Shader();
        Shader(RenderDriver* render);
        Shader(RenderDriver* render, std::string shaderFilename);
        
        ~Shader();
        
        void Init(std::string vertShaderFilename, std::string fragShaderFilename);
        
        void Begin();
        void End();
        
        bool Update();
        bool NeedsUpdate();
        
        void BindUniform(std::string token);
        void UploadUniform(std::string token, float* data, int verts);
        void UploadUniform(std::string token, glm::mat4 matrix);
        void UploadUniform(std::string token, float x, float y);
        
        void BindVertexAttrib(std::string token, int attribSize, int totalSize, int stride);
        
        bool IsLoaded() {
            return this->_loaded;
        }
        
    private:
        bool compile(const char* vertSource, const char* fragSource);
        
        bool checkProgramPointer();
        
        RenderDriver* _render;
        
        bool _loaded;
        
        std::map<std::string, uint> _uniforms;
        std::map<std::string, uint> _attribs;
        
        unsigned int _programPointer, _vertPointer, _fragPointer;
        std::string _vertFilename, _fragFilename;
        unsigned long _vertLastModify, _fragLastModify;
    };
}