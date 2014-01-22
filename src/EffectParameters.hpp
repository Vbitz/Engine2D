#pragma once

#include <vector>
#include <string>

#include "extern/json/json.h"

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