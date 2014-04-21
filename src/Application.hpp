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

#include "common.hpp"

#include "ScriptingManager.hpp"

#include "ResourceManager.hpp"

#include "vendor/GLFT_Font.hpp"

#include <queue>

#define ENGINE_ASSERT(value, msg) GetAppSingilton()->Assert(value, msg, __FILE__, __LINE__)

namespace Engine {
    
    class Application {
    public:
        int Start(int argc, char const *argv[]);
        
        bool IsDelayedConfig(std::string configKey);
      
        GLFT_Font* GetFont(std::string fontName, int fontSize);
        bool LoadFont(std::string prettyName, std::string filename);
        bool IsFontLoaded(std::string fontName);
        
        std::vector<std::string> GetCommandLineArgs();
        
        int GetScreenWidth();
        int GetScreenHeight();
        void SetScreenSize(int width, int height);
        
        Window* GetWindow();
        
        bool RunFile(std::string path, bool persist);
        
        void Exit();
        
        bool UsingGL3();
        
        void RunCommand(std::string str);
        
        void DetailProfile(int frames, std::string filename);
        
        void InvalidateScript(std::string filename);
        
        bool GetKeyPressed(int key);
        
        OpenGLVersion GetOpenGLVersion();
        
        EffectShaderTypes::Type GetBestEffectShaderType();
        
        static std::string GetEngineVersion();
        bool IsDebugMode();
        
        void UpdateScreen();
        
        void Assert(bool value, std::string reason, std::string line, int lineNumber);
        
        void AddScript(const char* filename_str, size_t filename_len);
        void DumpScripts();
        
    private:
        struct RawScriptInfo {
            const char* filename_str;
            size_t filename_len;
        };
        
        struct ScriptInfo {
            std::string filename;
        };
        
        void _mainLoop();
        
        int  _postStart();
        static EventMagic _dumpProfile(Json::Value args);
        static EventMagic _saveScreenshot(Json::Value args);
        static EventMagic _toggleFullscreen(Json::Value args);
        static EventMagic _restartRenderer(Json::Value args);
        bool _runFile(std::string path, bool persist);
        void _checkUpdate();
        void _loadTests();
        bool _parseCommandLine(int argc, const char* argv[]);
        void _applyDelayedConfigs();
        void _shutdownFonts();
        void _initFonts();
        void _shutdownOpenGL();
        void _initOpenGL();
        void _closeWindow();
        void _openWindow(int width, int height, bool fullscreen, std::string openGLVersion);
        void _loadBasicConfigs();
        void _hookConfigs();
        void _hookEvents();
        void _disablePreload();
        void _updateFrameTime();
        void _updateMousePos();
        void _shutdownScripting();
        void _printConfigVars();
        void _loadConfigFile();
        void _enableV8Debugger();
        void _handleDebugMessage();
        v8::Handle<v8::Context> _initScripting();
        v8::Local<v8::Object> _getScriptTable(std::string name);
        void _enableTypedArrays();
        void _enableHarmony();
        void _createEventMagic();
        void _initGLContext(GraphicsVersion v);
        
        void _processScripts();
        
        static EventMagic _rawInputHandler(Json::Value v);
        static EventMagic _rendererKillHandler(Json::Value v);
        static EventMagic _postCreateContext(Json::Value v);
        static EventMagic _rawResizeHandler(Json::Value v);
        
        bool _running = false;
        
        std::vector<std::string> _jsArgs;
        
        Window* _window = NULL;
        
        std::map<std::string, ResourceManager::FontResource*> _fonts;
        
        std::map<std::string, std::string> _delayedConfigs;
        
        std::map<std::string, long> _loadedFiles;
        
        std::vector<std::string> _archivePaths;
        
        bool _developerMode = false;
        bool _debugMode = false;
        bool _testMode = false;
        bool _configVarsMode = false;
        
        int _detailFrames = 0;
        std::string _detailFilename = "";
        
        std::queue<RawScriptInfo> _pendingScripts;
        std::unordered_map<std::string, ScriptInfo> _scripts;
    };
    
    Application* GetAppSingilton();
}