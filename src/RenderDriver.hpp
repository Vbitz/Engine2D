/*
 Filename: RenderDriver.hpp
 Purpose:  Generic Rendering Interface Spec
 
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

#include <limits>

#include "stdlib.hpp"

#include "RenderTypes.hpp"
#include "FontSheet.hpp"

#define GLM_FORCE_RADIANS
#include "vendor/glm/glm.hpp"

namespace Engine {
    ENGINE_CLASS(Texture);
    ENGINE_CLASS(FontSheet);
    ENGINE_CLASS(RenderDriver);
    
    class Drawable {
    public:
        virtual ~Drawable();
        
        virtual void Draw() = 0;
    protected:
        Drawable(RenderDriverPtr render) : _render(render) {}
        
        RenderDriverPtr _render;
        
        virtual void _init() {};
        virtual void _cleanup() {};
        
        friend class RenderDriver;
    };
    typedef Drawable* DrawablePtr;
    
    class RenderDriver {
    private:
        Color4f _currentColor = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
    public:
        class RenderDriverError {
        public:
            int Error;
            std::string Source, ErrorString;
            
            RenderDriverError(const char* source, int err, std::string errorString) : Source(source), Error(err), ErrorString(errorString) { }
        };
        
        virtual RendererType GetRendererType() = 0;
        virtual OpenGLVersion GetOpenGLVersion() = 0;
        virtual EffectShaderType GetBestEffectShaderType() = 0;
        
        virtual bool CheckError(const char* source) = 0;
        
        virtual bool HasExtention(std::string extentionName)= 0;
        virtual std::vector<std::string> GetExtentions() = 0;
        
        virtual void ResetMatrix() = 0;
        
        virtual void BeginRendering(PolygonMode mode) = 0;
        
        virtual void EndRendering() = 0;
        
        inline void AddVert(float x, float y, float z) {
            this->AddVert(x, y, z, this->_currentColor, 0.0, 0.0);
        }
        inline void AddVert(float x, float y, float z, Color4f col) {
            this->AddVert(x, y, z, col, 0.0, 0.0);
        }
        inline void AddVert(float x, float y, float z, float s, float t) {
            this->AddVert(x, y, z, _currentColor, s, t);
        }
        inline void AddVert(float x, float y, float z, Color4f col, float s, float t) {
            this->_addVert(glm::vec3(x, y, z), col, glm::vec2(s, t), glm::vec3());
        };
        inline void AddVert(glm::vec3 pos, Color4f col, glm::vec2 uv, glm::vec3 normal) {
            this->_addVert(pos, col, uv, normal);
        }
        
        virtual void EnableTexture(TexturePtr texId) = 0;
        virtual void DisableTexture() = 0;
        
        virtual void EnableSmooth() = 0;
        virtual void DisableSmooth() = 0;
        
        virtual void SetLineWidth(float value) = 0;
        
        void Print(float x, float y, const char* string);
        
        float CalcStringWidth(std::string str);
        void SetFont(std::string name, int fontSize);
        void LoadFont(std::string prettyName, std::string filename);
        bool IsFontLoaded(std::string name);
        
        virtual void FlushAll() = 0;
        
        virtual void Init2d() = 0;
        
        virtual void Clear() = 0;
        
		virtual void Begin2d() = 0;
		virtual void End2d() = 0;
        
        virtual void Reset() = 0;
        
        void ClearColor(Color4f col);
        void ClearColor(int col);
        void ClearColor(std::string colorName);
        void ClearColor(float r, float g, float b);
        
        Color4f GetColor();
        
        void SetColor(Color4f col);
        void SetColor(int col);
        void SetColor(std::string colorName);
        void SetColor(float r, float g, float b);
        void SetColor(float r, float g, float b, float a);
        
        virtual void Set(RenderStateFlag flag, bool enable) = 0;
        
        virtual void SetCenter(float x, float y) = 0;
        virtual void CameraPan(float x, float y) = 0;
        virtual void CameraZoom(float f) = 0;
        virtual void CameraRotate(float r) = 0;
        
        template<class T> inline auto CreateDrawable() -> T* {
            DrawablePtr drawable = new T(this);
            this->_managedDrawables.push_back(drawable);
            drawable->_init();
            return static_cast<T*>(drawable);
        }
    
    protected:
        void _cleanupDrawable(DrawablePtr drawable);
        
        virtual void _clearColor(Color4f col) = 0;
        virtual void _addVert(glm::vec3 pos, Color4f col, glm::vec2 uv, glm::vec3 normal) = 0;
        
        FontSheetPtr _getSheet(std::string fontName);
        
        std::string _currentFontName = "basic";
        int _currentFontSize = 16;
        
        std::vector<DrawablePtr> _managedDrawables;
        
    private:
        std::unordered_map<std::string, FontSheetPtr> _sheets;
        FontSheetPtr _sheet = NULL;
        
        friend class Drawable;
        friend class RenderDriverDrawProfiler;
    };
}