/*
   Filename: Application.hpp
   Purpose:  Main application logic and management

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

#include "Window.hpp"
#include "Addon.hpp"

#include "EngineUI.hpp"

#include "ScriptingManager.hpp"

#include "ResourceManager.hpp"

#include "Events.hpp"

#include "Drawables/CubeDrawableTest.hpp"

#include <queue>

#define ENGINE_ASSERT(value, msg) GetAppSingilton()->Assert(value, msg, __FILE__, __LINE__)

namespace Engine {
    bool HasGLContext();
    
    void EnableGLContext();
    
    void DisableGLContext();
    
    ENGINE_CLASS(EngineUI);
    
    ENGINE_CLASS(Application);
    
    ApplicationPtr GetAppSingilton();
    
    int EngineMain(int argc, char const *argv[]);
    
    class Application {
    public:
        // Startup/Shutdown
        int Start(int argc, char const *argv[]);
        void Exit();
        
        // Public application functions
        std::vector<std::string> GetCommandLineArgs();
        bool IsDelayedConfig(std::string configKey);
        
        // Public window functions
        WindowPtr GetWindow() {
            return this->_window;
        }
        bool GetKeyPressed(int key);
        void UpdateScreen();
        RenderDriverPtr GetRender() {
            ENGINE_ASSERT(this->_renderGL != NULL, "RenderGL3 is not initalized");
            return _renderGL;
        }
        
        // Public EngineUI functions
        EngineUIPtr GetEngineUI() {
            return this->_engineUI;
        }
        
        // Public Scripting functions
        ScriptingManager::ContextPtr GetScriptingContext() {
            return this->_scripting;
        }
        
        // Public Scripting support functions
        void AddScript(const char* filename_str, size_t filename_len);
        void DumpScripts();
        
        // Public debug functions
        void DetailProfile(int frames, std::string filename);
        bool IsDebugMode();
        bool IsDeveloperMode();
        void Assert(bool value, std::string reason, std::string line, int lineNumber);
        
        LoadOrder GetCurrentLoadState() {
            return this->_currentLoadingState;
        }
        
        // Static functions
        static std::string GetEngineVersion();
        
    private:
        struct RawScriptInfo {
            const char* filename_str;
            size_t filename_len;
        };
        
        struct ScriptInfo {
            std::string filename;
        };
        
        // Main Loop
        void _mainLoop();
        void _updateFrameTime();
        void _updateMousePos();
        void _processScripts();
        
        // System Events
        static EventMagic _dumpProfile(Json::Value args);
        static EventMagic _saveScreenshot(Json::Value args);
        static EventMagic _toggleFullscreen(Json::Value args);
        static EventMagic _restartRenderer(Json::Value args);
        static EventMagic _dumpLog(Json::Value args);
        static EventMagic _appEvent_Exit(Json::Value v);
        static EventMagic _appEvent_DumpScripts(Json::Value v);
        static EventMagic _doDrawProfile(Json::Value v);
        static EventMagic _requireDynamicLibary(Json::Value v);
        
        // Window Events
        static EventMagic _rawInputHandler(Json::Value v);
        static EventMagic _rendererKillHandler(Json::Value v);
        static EventMagic _postCreateContext(Json::Value v);
        static EventMagic _rawResizeHandler(Json::Value v);
        
        // Config Events
        static EventMagic _config_CoreRenderAA(Json::Value args);
        static EventMagic _config_CoreWindowVSync(Json::Value args);
        static EventMagic _config_CoreWindowSize(Json::Value args);
        static EventMagic _config_CoreWindowTitle(Json::Value args);
        
        // Testing
        void _loadTests();
        
        // Init
        int  _postStart();
        bool _parseCommandLine(int argc, const char* argv[]);
        void _applyDelayedConfigs();
        void _loadBasicConfigs();
        void _hookConfigs();
        void _hookEvents();
        void _printConfigVars();
        void _loadConfigFile();
        void _disablePreload();
        void _updateAddonLoad(LoadOrder load);
        
        // OpenGL
        void _shutdownOpenGL();
        void _initOpenGL();
        void _initGLContext(GraphicsVersion v);
        
        // Windowing
        void _closeWindow();
        void _openWindow(int width, int height, bool fullscreen, std::string openGLVersion);
        
        // Modes
        bool _developerMode = false;
        bool _debugMode = false;
        bool _testMode = false;
        bool _configVarsMode = false;
        
        // Vars
        bool _running = false;
        LoadOrder _currentLoadingState = LoadOrder::Startup;
        
        std::vector<std::string> _jsArgs;
        
        WindowPtr _window = NULL;
        EngineUIPtr _engineUI = NULL;
        RenderDriverPtr _renderGL = NULL;
        ScriptingManager::ContextPtr _scripting = NULL;
        
        std::map<std::string, std::string> _delayedConfigs;
        
        std::vector<std::string> _archivePaths;
        
        int _detailFrames = 0;
        std::string _detailFilename = "";
        
        std::queue<RawScriptInfo> _pendingScripts;
        std::unordered_map<std::string, ScriptInfo> _scripts;
        
        //Drawables::CubeDrawableTest* _cubeTest;
        
        long _frames = 0;
    };
}