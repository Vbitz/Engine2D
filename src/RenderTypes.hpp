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
        std::string glslVersion, glewVersion, glVendor, glRenderer, fullGLVersion;
        
        std::string toString() {
            std::stringstream glVersion;
            glVersion << this->major << "." << this->minor << "." << this->revision;
            return glVersion.str();
        }
    };
    
    enum MouseButton {
        MouseButton_Left = 0,
        MouseButton_Right = 1,
        MouseButton_Middle
    };
    
    enum GraphicsVersion {
        Graphics_OpenGL_Modern,
        Graphics_OpenGL_Legacy
    };
    
    enum KeyStatus {
        Key_Release = 0,
        Key_Press = 1,
        Key_Repeat = 2
    };
    
    enum Keys {
        Key_Backspace = 259,
        Key_Enter = 257,
        Key_Up = 265,
        Key_Down = 264,
#ifndef _PLATFORM_WIN32
        Key_Console = 161,
#else
        Key_Console = 96,
#endif
        Key_F1 = 290,
        Key_F2 = 291,
        Key_F3 = 292,
        Key_F4 = 293,
        Key_F5 = 294,
        Key_F6 = 295,
        Key_F7 = 296,
        Key_F8 = 297,
        Key_F9 = 298,
        Key_F10 = 299,
        Key_F11 = 300,
        Key_F12 = 301,
        Key_F13 = 302,
        Key_F14 = 303,
        Key_F15 = 304,
        Key_F16 = 305,
        Key_F17 = 306,
        Key_F18 = 307,
        Key_F19 = 308,
        Key_F20 = 309,
        Key_F21 = 310,
        Key_F22 = 311,
        Key_F23 = 312,
        Key_F24 = 313,
        Key_F25 = 314,
        Key_Null
    };
    
    enum PolygonMode {
        PolygonMode_Invalid,
        PolygonMode_LineStrip,
        PolygonMode_Triangles,
        PolygonMode_LineLoop,
        PolygonMode_Lines,
        PolygonMode_TriangleStrip,
        PolygonMode_TriangleFan
    };
    
    namespace EffectShaderTypes {
        enum Type {
            GLSL_150,
            Unknown
        };
    }
}

