/*
 Filename: RenderTypes.hpp
 Purpose:  Common Renderer Enums
 
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

#include <string>
#include <sstream>

namespace Engine {
    enum RendererType {
        RendererType_OpenGL3,
        RendererType_OpenGL2
    };
    
    struct OpenGLVersion {
        int major, minor, revision;
        std::string glslVersion, glewVersion;
        
        std::string toString() {
            std::stringstream glVersion;
            glVersion << this->major << "." << this->minor << "." << this->revision;
            return glVersion.str();
        }
    };
    
    enum MouseButton {
        MouseButton_Left,
        MouseButton_Right,
        MouseButton_Middle
    };
    
    enum GraphicsVersion {
        Graphics_OpenGL_Modern,
        Graphics_OpenGL_Legacy
    };
    
    enum KeyStatus {
        Key_Press,
        Key_Release,
        Key_Repeat
    };
    
    namespace EffectShaderTypes {
        enum Type {
            GLSL_150,
            Unknown
        };
    }
}

