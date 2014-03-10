/*
   Filename: EffectParameters.hpp
   Purpose:  Parses formatting for shader specfication

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
#include <string>

#include "vendor/json/json.h"

#include "common.hpp"

#include "Shader.hpp"

namespace Engine {
    
    typedef struct {
        EffectShaderTypes::Type type;
        std::string vertexShaderPath;
        std::string fragmentShaderPath;
    } ShaderSpec;
    
    typedef struct {
        std::string vertexParam, colorParam, texCoardParam,
                    modelMatrixParam, viewMatrixParam, projectionMatrixParam;
    } ShaderSettings;
    
    class EffectParameters {
    public:
        EffectParameters();
        EffectParameters(std::string basePath, Json::Value root);
        
        Shader* CreateShader();
        ShaderSettings GetShaderSettings();
        
        bool NeedsUpdate();
        
    private:
        void _read(std::string basePath, Json::Value root);
        ShaderSpec _getBestShaderSpec();
        
        std::vector<ShaderSpec> _shaders;
        int _textureCount;
        ShaderSettings _shaderSettings;
        
        Json::Value _userParams;
    };
    
    namespace EffectReader {
        EffectParameters GetEffectFromFile(std::string filename);
    }
}