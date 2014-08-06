/*
   Filename: Shader.cpp
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

#define GLEW_STATIC
#include "vendor/GL/glew.h"

#include "Shader.hpp"

#include "Filesystem.hpp"

#include "Profiler.hpp"

#include "Util.hpp"
#include "Platform.hpp"

#include "Application.hpp"

namespace Engine {
    #define EFFECT_SHADER_TYPE(str,enum) if (type == str) return enum;
    
    EffectShaderType shaderTypeFromString(std::string type) {
             EFFECT_SHADER_TYPE("glsl_110", EffectShaderType::GLSL_110)
        else EFFECT_SHADER_TYPE("glsl_120", EffectShaderType::GLSL_120)
        else EFFECT_SHADER_TYPE("glsl_130", EffectShaderType::GLSL_130)
        else EFFECT_SHADER_TYPE("glsl_140", EffectShaderType::GLSL_140)
        else EFFECT_SHADER_TYPE("glsl_150", EffectShaderType::GLSL_150)
        else EFFECT_SHADER_TYPE("glsl_330", EffectShaderType::GLSL_330)
        else EFFECT_SHADER_TYPE("glsl_400", EffectShaderType::GLSL_400)
        else EFFECT_SHADER_TYPE("glsl_410", EffectShaderType::GLSL_410)
        else EFFECT_SHADER_TYPE("glsl_420", EffectShaderType::GLSL_420)
        else EFFECT_SHADER_TYPE("glsl_430", EffectShaderType::GLSL_430)
        else EFFECT_SHADER_TYPE("glsl_440", EffectShaderType::GLSL_440)
        else return EffectShaderType::Unknown;
    }
    
    std::string resolvePath(std::string basePath, std::string path) {
        if (path.find_first_of('/') == 0)
            return path;
        else
            return basePath + path;
    }
    
    ShaderSpec readShaderSpec(std::string basePath, Json::Value spec) {
        ShaderSpec ret;
        ret.type = shaderTypeFromString(spec["type"].asString());
        ret.vertexShaderPath = resolvePath(basePath, spec["vertex"].asString());
        ret.fragmentShaderPath = resolvePath(basePath, spec["fragment"].asString());
        return ret;
    }
    
    EffectParameters::EffectParameters() {
        
    }
    
    EffectParameters::EffectParameters(std::string basePath, Json::Value root) {
        this->_read(basePath, root);
    }
    
    Shader* EffectParameters::CreateShader() {
        ShaderSpec spec = this->_getBestShaderSpec();
        ShaderPtr shader = new Shader(GetAppSingilton()->GetRender());
        Logger::begin("EffectParameters", Logger::LogLevel_Verbose) << "Using Shader: " << spec.vertexShaderPath << " : " << spec.fragmentShaderPath << Logger::end();
        shader->Init(spec.type, spec.vertexShaderPath, spec.fragmentShaderPath);
        return shader;
    }
    
    ShaderSettings EffectParameters::GetShaderSettings() {
        return _shaderSettings;
    }
    
    bool EffectParameters::NeedsUpdate() {
        return false;
    }
    
    void EffectParameters::_read(std::string basePath, Json::Value root) {
        Json::Value shadersNode = root["shader"];
        for (int i = 0; i < shadersNode.size(); i++) {
            this->_shaders.push_back(readShaderSpec(basePath, shadersNode[i]));
        }
        
        Json::Value vertexBufferParams = root["vertexBufferParams"];
        Json::Value cameraSettings = root["cameraSettings"];
        
        this->_textureCount = root["textures"].asInt();
        this->_shaderSettings.vertexParam = vertexBufferParams["vertex"].asString();
        this->_shaderSettings.colorParam = vertexBufferParams["color"].asString();
        this->_shaderSettings.texCoardParam = vertexBufferParams["texCoard"].asString();
        
        this->_shaderSettings.modelMatrixParam = cameraSettings["model"].asString();
        this->_shaderSettings.viewMatrixParam = cameraSettings["view"].asString();
        this->_shaderSettings.projectionMatrixParam = cameraSettings["projection"].asString();
        
        this->_userParams = root["userParams"];
    }
    
    ShaderSpec EffectParameters::_getBestShaderSpec() {
        EffectShaderType bestType = GetAppSingilton()->GetRender()->GetBestEffectShaderType();
        for (auto iter = this->_shaders.begin(); iter != this->_shaders.end(); iter++) {
            if (iter->type == bestType) {
                return *iter;
            }
        }
        return this->_shaders.at(0);
    }
    
    namespace EffectReader {
        std::string getBasePath(std::string path) {
            return path.substr(0, path.find_last_of('/') + 1);
        }
        
        EffectParametersPtr GetEffectFromFile(std::string filename) {
            std::string fileContent = std::string(Filesystem::GetFileContent(filename));
            
            Json::Reader reader;
            Json::Value root;
            
            reader.parse(fileContent, root);
            
            return new EffectParameters(getBasePath(filename), root);
        }
    }

    
    std::string _getVersionString(EffectShaderType type) {
        switch (type) {
            case EffectShaderType::GLSL_120: return "#version 120\n";
            case EffectShaderType::GLSL_140: return "#version 140\n";
            case EffectShaderType::GLSL_150: return "#version 150\n";
            default:
                return "";
        }
    }
    
    Shader::Shader() : _loaded(false) {
        
    }
    
    Shader::Shader(RenderDriverPtr render) : _render(render) {
    }
    
    Shader::~Shader() {
        if (!this->_loaded && Engine::HasGLContext()) {
            this->End();
            glDeleteShader(this->_vertPointer);
            glDeleteShader(this->_fragPointer);
            glDeleteProgram(this->_programPointer);
        }
    }
    
    void Shader::Begin() {
        if (this->checkProgramPointer()) {
            ENGINE_PROFILER_SCOPE;
            glUseProgram(this->_programPointer);
        }
    }
    
    void Shader::End() {
        ENGINE_PROFILER_SCOPE;
        glUseProgram(0);
    }
    
    bool Shader::Update() {
        if (this->NeedsUpdate()) {
            ENGINE_PROFILER_SCOPE;
            
            this->_render->CheckError("Shader::Update::Pre");
            
            if (glIsShader(this->_vertPointer)) {
                glDeleteShader(this->_vertPointer);
            }
            
            if (glIsShader(this->_fragPointer)) {
                glDeleteShader(this->_fragPointer);
            }
            
            if (glIsProgram(this->_programPointer)) {
                glDeleteProgram(this->_programPointer);
            }
            
            this->_render->CheckError("Shader::Update::PostDelete");
            
            this->_loaded = false;
            this->Init(this->_type, this->_vertFilename, this->_fragFilename);
            
            this->_render->CheckError("Shader::Update::PostInit");
            
            return true;
        } else {
            return false;
        }
    }
    
    bool Shader::NeedsUpdate() {
        static float lastUpdate = Platform::GetTime();
        
        if (!glIsProgram(this->_programPointer)) {
            return true; // The shader is invalid so it needs to be loaded
        }
        
        if ((Platform::GetTime() - lastUpdate) > 0.2) {
            lastUpdate = Platform::GetTime();
            return Filesystem::GetFileModifyTime(this->_vertFilename) > this->_vertLastModify
            || Filesystem::GetFileModifyTime(this->_fragFilename) > this->_fragLastModify;
        } else {
            return false;
        }
    }
    
    bool Shader::checkProgramPointer() {
        return this->_programPointer != 0 && this->_loaded && glIsProgram(this->_programPointer);
    }
    
    void Shader::BindUniform(std::string token) {
        if (this->checkProgramPointer()) {
            this->Begin();
            this->_uniforms[token] = glGetUniformLocation(this->_programPointer, token.c_str());
            this->End();
        }
    }
    
    void Shader::UploadUniform(std::string token, float x, float y) {
        if (this->checkProgramPointer()) {
            ENGINE_PROFILER_SCOPE;
            glUniform2f(this->_uniforms[token], x, y);
        }
    }
    
    void Shader::UploadUniform(std::string token, glm::mat4 matrix) {
        if (this->checkProgramPointer()) {
            if (this->_matrix_uniform_cache.count(token) == 0 || this->_matrix_uniform_cache[token] != matrix) {
                ENGINE_PROFILER_SCOPE;
                glUniformMatrix4fv(this->_uniforms[token], 1, GL_FALSE, &matrix[0][0]);
                this->_matrix_uniform_cache[token] = matrix;
            }
        }
    }
    
    void Shader::UploadUniform(std::string token, float* data, int verts) {
        if (this->checkProgramPointer()) {
            ENGINE_PROFILER_SCOPE;
            glUniform2fv(this->_uniforms[token], verts, data);
        }
    }
    
    void Shader::BindVertexAttrib(std::string token, int attribSize, int totalSize, int stride) {
        if (!this->checkProgramPointer()) {
            return;
        }
        
        ENGINE_PROFILER_SCOPE;
        
        GLuint attribPos = glGetAttribLocation(_programPointer, token.c_str());
        
        if ((int) attribPos == -1) {
            Logger::begin("Shader", Logger::LogLevel_Error) << "Bad token name: " << token << Logger::end();
        }
        
        this->_render->CheckError("Shader::BindVertexAttrib::PostGetAttribLocation");
        
        glVertexAttribPointer(attribPos, attribSize, GL_FLOAT, GL_FALSE,
                              totalSize * sizeof(float),
                              stride == 0 ? NULL : (void*)(stride * sizeof(float)));
        
        this->_render->CheckError("Shader::BindVertexAttrib::PostVertexAttribPointer");
        
        glEnableVertexAttribArray(attribPos);
        
        this->_render->CheckError("Shader::BindVertexAttrib::PostEnable");
    }
    
    void Shader::SetMacro(std::string name, ShaderType exposure) {
        if (exposure == ShaderType::All || exposure == ShaderType::VertexShader) {
            this->_vertexMacros.push_back(name);
        }
        if (exposure == ShaderType::All || exposure == ShaderType::FragmentShader) {
            this->_fragMacros.push_back(name);
        }
    }
    
    void Shader::Init(EffectShaderType type, std::string vertShaderFilename, std::string fragShaderFilename) {
        ENGINE_PROFILER_SCOPE;
        
        this->_type = type;
        
        this->_vertFilename = vertShaderFilename;
        this->_fragFilename = fragShaderFilename;
        char* vertShader = Filesystem::GetFileContent(vertShaderFilename);
        char* fragShader = Filesystem::GetFileContent(fragShaderFilename);
        
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
        
        std::free(vertShader);
        std::free(fragShader);
        
        this->_loaded = true;
    }
    
    bool Shader::compile(const char* vertSource, const char* fragSource) {
        ENGINE_PROFILER_SCOPE;
        
        this->_vertPointer = glCreateShader(GL_VERTEX_SHADER);
        this->_fragPointer = glCreateShader(GL_FRAGMENT_SHADER);
        
        std::string versionString = _getVersionString(this->_type);
        
        std::stringstream vertBuilder, fragBuilder;
        
        for (auto iter = this->_vertexMacros.begin(); iter != this->_vertexMacros.end(); iter++) {
            vertBuilder << "#define " << &iter << '\n';
        }
        
        for (auto iter = this->_fragMacros.begin(); iter != this->_fragMacros.end(); iter++) {
            vertBuilder << "#define " << &iter << '\n';
        }
        
        const char *vertSources[3] = {
            versionString.c_str(),
            vertBuilder.str().c_str(),
            vertSource
        };
        
        glShaderSource(this->_vertPointer, 3, vertSources, NULL);
      
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
        
        const char *fragSources[3] = {
            versionString.c_str(),
            fragBuilder.str().c_str(),
            fragSource
        };
        
        glShaderSource(this->_fragPointer, 3, fragSources, NULL);
        
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