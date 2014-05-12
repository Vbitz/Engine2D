/*
   Filename: Window.hpp
   Purpose:  Window creation and management

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

#define GLEW_STATIC
#include "vendor/GL/glew.h"
#include <glm/glm.hpp>

#include "RenderTypes.hpp"

namespace Engine {
    
    class Window {
    public:
        Window(GraphicsVersion v) : _version(v) { this->_init(); }
        ~Window() { this->_destroy(); }
        
        virtual void Show() = 0;
        virtual void Hide() = 0;
        
        virtual void Begin() = 0;
        virtual void End() = 0;
        
        virtual void Present() = 0;
        
        virtual void Reset() = 0;
        
        virtual glm::vec2 GetCursorPos() = 0;
        virtual bool GetMouseButtonPressed(MouseButton b) = 0;
        
        virtual KeyStatus GetKeyStatus(int key) = 0;
        
        virtual glm::vec2 GetWindowSize() = 0;
        virtual void SetWindowSize(glm::vec2 s) = 0;
        
        virtual bool GetVisible() = 0;
        virtual bool IsFocused() = 0;
        virtual bool ShouldClose() = 0;
        
        virtual void WaitEvents() = 0;
        
        virtual std::string GetTitle() = 0;
        virtual void SetTitle(std::string title) = 0;
        
        virtual bool GetFullscreen() = 0;
        virtual void SetFullscreen(bool fullscreen) = 0;
        
        virtual void SetVSync(bool vSync) = 0;
        
        virtual void SetAntiAlias(int samples) = 0;
        
        virtual void SetDebug(bool debug) = 0;
        
        // This is just a tempoary stopgap until the API is more graphics agnostic
        virtual OpenGLVersion GetGlVersion() = 0;
        
        GraphicsVersion GetGraphicsVersion() {
            return this->_version;
        }
        
    protected:
        virtual void _init() {};
        virtual void _destroy() {};
        
        GraphicsVersion _version;
    };
    
    Window* CreateWindow(GraphicsVersion v);
}