/*
   Filename: Window_sdl.cpp
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

#include "Window.hpp"

#define GLEW_STATIC
#include "vendor/GL/glew.h"

#include <SDL.h>

#include "RenderGL3.hpp"

#include "Events.hpp"
#include "Logger.hpp"

#define GLM_FORCE_RADIANS
#include "vendor/glm/gtc/matrix_transform.hpp"

#include "Platform.hpp"

namespace Engine {
    std::string _getMouseButtonString(char index) {
        switch (index) {
            case SDL_BUTTON_LEFT:
                return "mouseLeft";
            case SDL_BUTTON_RIGHT:
                return "mouseRight";
            case SDL_BUTTON_MIDDLE:
                return "mouseMiddle";
            case SDL_BUTTON_X1:
                return "mouse4";
            case SDL_BUTTON_X2:
                return "mouse5";
            default:
                break;
        }
    }
    
    int _translateKeyCodeToKeys(int key) {
        switch (key) {
            case SDLK_BACKSPACE:    return Key_Backspace;
            case SDLK_RETURN:       return Key_Enter;
            case SDLK_UP:           return Key_Up;
            case SDLK_DOWN:         return Key_Up;
            case 96:                return Key_Console;
            case SDLK_F1:           return Key_F1;
            case SDLK_F2:           return Key_F2;
            case SDLK_F3:           return Key_F3;
            case SDLK_F4:           return Key_F4;
            case SDLK_F5:           return Key_F5;
            case SDLK_F6:           return Key_F6;
            case SDLK_F7:           return Key_F7;
            case SDLK_F8:           return Key_F8;
            case SDLK_F9:           return Key_F9;
            case SDLK_F10:          return Key_F10;
            case SDLK_F11:          return Key_F11;
            case SDLK_F12:          return Key_F12;
            case SDLK_F13:          return Key_F13;
            case SDLK_F14:          return Key_F14;
            case SDLK_F15:          return Key_F15;
            case SDLK_F16:          return Key_F16;
            case SDLK_F17:          return Key_F17;
            case SDLK_F18:          return Key_F18;
            case SDLK_F19:          return Key_F19;
            case SDLK_F20:          return Key_F20;
            case SDLK_F21:          return Key_F21;
            case SDLK_F22:          return Key_F22;
            case SDLK_F23:          return Key_F23;
            case SDLK_F24:          return Key_F24;
            default:                return key;
        }
    }
    
    class Window_sdl : public Window {
    public:
        
        Window_sdl(GraphicsVersion v) : Window(v) { this->_init(); }
        
        void Show() override {
            if (this->_window == NULL) {
                this->_create();
            }
            assert(this->_window != NULL);
            this->_visible = true;
            SDL_ShowWindow(this->_window);
        }
        
        void Hide() override {
            assert(this->_window != NULL);
            this->_visible = false;
            SDL_HideWindow(this->_window);
        }
        
        void Begin() override {
            assert(this->_window != NULL);
            SDL_GL_MakeCurrent(this->_window, this->_context);
        }
        
        void End() override {
            // TODO: Implament for SDL
        }
        
        void Present() override {
            SDL_GL_SwapWindow(this->_window);
            this->_pumpEvents();
        }
        
        virtual void Reset() override {
            this->_destroy();
            this->_create();
        }
        
        glm::vec2 GetCursorPos() override {
            int xPos, yPos;
            SDL_GetMouseState(&xPos, &yPos);
            return glm::vec2(xPos, yPos);
        }
        
        bool GetMouseButtonPressed(MouseButton b) override {
            return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(b);
        }
        
        void SetCaptureMouse(bool capture) override {
            SDL_SetRelativeMouseMode(capture ? SDL_TRUE : SDL_FALSE);
        }
        
        KeyStatus GetKeyStatus(int key) override {
            return SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromKey(key)] > 0 ? Key_Press : Key_Release;
        }
        
        glm::vec2 GetWindowSize() override {
            return this->_size;
        }
        
        bool GetVisible() override {
            return this->_visible;
        }
        
        bool IsFocused() override {
            return this->_mouseFocus || this->_keyboardFocus;
        }
        
        bool ShouldClose() override {
            return this->_quit;
        }
        
        void WaitEvents() override {
            SDL_WaitEvent(NULL);
        }
        
        void SetWindowSize(glm::vec2 s) override {
            this->_size = s;
            SDL_SetWindowSize(this->_window, s.x, s.y);
        }
        
        std::string GetTitle() override {
            return this->_title;
        }
        
        void SetTitle(std::string title) override {
            this->_title = title;
            SDL_SetWindowTitle(this->_window, title.c_str());
        }
        
        bool GetFullscreen() override {
            return this->_fullscreen;
        }
        
        void SetFullscreen(bool fullscreen) override {
            this->_fullscreen = fullscreen;
            if (this->_window != NULL) {
                this->Reset();
            }
        }
        
        void SetVSync(bool vSync) override {
            this->_vSync = vSync;
            SDL_GL_SetSwapInterval(this->_vSync ? 1 : 0);
        }
        
        void SetAntiAlias(int samples) override {
            this->_aaSamples = samples;
        }
        
        void SetDebug(bool debug) override {
            this->_debug = debug;
        }
        
        OpenGLVersion GetGlVersion() override {
            assert(this->_window != NULL);
            OpenGLVersion ret;

            // TODO: Implament for SDL
            
            if (this->_version == GraphicsVersion::OpenGL_Modern) {
                glGetIntegerv(GL_MAJOR_VERSION, &ret.major);
                glGetIntegerv(GL_MINOR_VERSION, &ret.minor);
                ret.revision = 0;
            } else {
                std::string version = std::string((const char*) glGetString(GL_VERSION));
                version = version.substr(0, version.find_first_of(' '));
                std::string majorVersion = version.substr(0, version.find_first_of('.'));
                std::string minorVersion = version.substr(majorVersion.length() + 1,
                            version.find_first_of('.', majorVersion.length() + 1));
                ret.major = std::stoi(majorVersion);
                ret.minor = std::stoi(minorVersion);
            }
            
            ret.glslVersion = (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION);
            ret.glewVersion = (const char*) glewGetString(GLEW_VERSION);
            
            return ret;
        }
        
        std::string GetWindowVersion() override {
            return "SDL v2";
        }
        
        int GetMaxTextureSize() override {
            GLint result = 0;
            
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, &result);
            
            return result;
        }
        
        glm::mat4 GetOrthoProjection() override {
            glm::vec2 window = this->GetWindowSize();
            return glm::ortho(0.0f, window.x, window.y, 0.0f, 1000.0f, -1000.0f);
        }
        
        RenderDriver* GetRender() override {
            return this->_render;
        }
        
    private:
        void _init() override {
            
        }
        
        void _destroy() override {
            SDL_GL_DeleteContext(this->_context);
            SDL_DestroyWindow(this->_window);
            GetEventsSingilton()->GetEvent("destroyWindow")->Emit();
        }
        
        void _resizeCallback(int width, int height) {
            this->_size = glm::vec2(width, height);
            
            Json::Value val(Json::objectValue);
            
            val["width"] = width;
            val["height"] = height;
            
            glViewport(0, 0, width, height);
            
            GetEventsSingilton()->GetEvent("rawResize")->Emit(val);
        }
        
        void _keypressCallback(SDL_KeyboardEvent k) {
            Json::Value val(Json::objectValue);
            
            val["rawKey"] = _translateKeyCodeToKeys(k.keysym.sym);
            val["key"] = SDL_GetKeyName(k.keysym.sym);
            if (k.repeat > 0) {
                val["rawPress"] = Key_Repeat;
                val["state"] = "repeat";
            } else {
                val["rawPress"] = k.state == SDL_PRESSED ? Key_Press : Key_Release;
                val["state"] =  k.state == SDL_PRESSED ? "press" : "release";
            }
            
            val["shift"] = k.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT);
            
            GetEventsSingilton()->GetEvent("rawInput")->Emit(val);
        }
        
        void _mouseButtonCallback(SDL_MouseButtonEvent b) {
            Json::Value val(Json::objectValue);
            
            val["button"] = _getMouseButtonString(b.button);
            val["action"] = b.state == SDL_PRESSED ? "press" : "release";
            val["rawMods"] = 0;
            
            val["x"] = b.x;
            val["y"] = b.y;
            
            GetEventsSingilton()->GetEvent("mouseButton")->Emit(val);
        }
        
        void _handleWindowEvent(SDL_WindowEvent e) {
            switch (e.event) {
                case SDL_WINDOWEVENT_RESIZED:
                    this->_resizeCallback(e.data1, e.data2);
                    break;
                case SDL_WINDOWEVENT_ENTER:
                    this->_mouseFocus = true;
                    break;
                case SDL_WINDOWEVENT_LEAVE:
                    this->_mouseFocus = false;
                    break;
                case SDL_WINDOWEVENT_FOCUS_GAINED:
                    this->_keyboardFocus = true;
                    break;
                case SDL_WINDOWEVENT_FOCUS_LOST:
                    this->_keyboardFocus = false;
                    break;
                default:
                    break;
            }
        }

        void _pumpEvents() {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_QUIT:
                        this->_quit = true;
                        break;
                    case SDL_WINDOWEVENT:
                        this->_handleWindowEvent(e.window);
                        break;
                    case SDL_KEYDOWN:
                    case SDL_KEYUP:
                        this->_keypressCallback(e.key);
                        break;
                    case SDL_MOUSEBUTTONUP:
                    case SDL_MOUSEBUTTONDOWN:
                        this->_mouseButtonCallback(e.button);
                    default:
                        break;
                }
            }
        }
        
        void _create() {
            if (this->_version == GraphicsVersion::OpenGL_Modern) {
#ifdef _PLATFORM_OSX
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
            } else if (this->_version == GraphicsVersion::OpenGL_Legacy) {
                
            }

            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            
            this->_window = SDL_CreateWindow(this->_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->_size.x, this->_size.y, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
            
            if (this->_window == NULL) {
                Logger::begin("Window", Logger::LogLevel_Error) << "Error Creating Window" << Logger::end();
                return;
            }

            this->_context = SDL_GL_CreateContext(this->_window);
            
            if (this->_context == NULL) {
                Logger::begin("Window", Logger::LogLevel_Error) << "Error Creating Context" << Logger::end();
            }
            
            this->Begin();
            
            SDL_GL_SetSwapInterval(this->_vSync ? 1 : 0);
            
            glewExperimental = GL_TRUE;
            
            GLenum err = glewInit();
            
            if (err != GLEW_OK) {
                Logger::begin("Window", Logger::LogLevel_Error) << "Error starting GLEW: " << glewGetErrorString(err) << Logger::end();
                this->_destroy();
                return;
            }
            
            glGetError(); // GLEW always causes a GL error when it gets extentions
            
            switch (this->_version) {
                case GraphicsVersion::OpenGL_Modern:
                case GraphicsVersion::OpenGL_Legacy:
                    this->_render = CreateRenderGL3();
                    break;
            }
            
            GetEventsSingilton()->GetEvent("postCreateContext")->Emit();
        }
        
        RenderDriver* _render = NULL;
        SDL_Window* _window = NULL;
        SDL_GLContext _context;
        
        glm::vec2 _size = glm::vec2(800, 600);
        bool _quit = false;
        bool _visible = false;
        bool _mouseFocus = true;
        bool _keyboardFocus = false;
        bool _fullscreen = false;
        std::string _title = "Engine2D";
        int _aaSamples = 0;
        bool _debug = false;
        bool _vSync = true;
    };
    
    void Window::StaticInit() {
        SDL_Init(SDL_INIT_VIDEO);
    }
    
    void Window::StaticDestroy() {
        SDL_Quit();
    }
    
    Window* CreateWindow(GraphicsVersion v) {
        return new Window_sdl(v);
    }
}