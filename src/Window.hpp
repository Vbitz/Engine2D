#pragma once

#include <string>

#define GLEW_STATIC
#include "vendor/GL/glew.h"
#include <glm/glm.hpp>

namespace Engine {
    typedef struct _openGLVersion {
        int major, minor, revision;
    } OpenGLVersion;
    
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