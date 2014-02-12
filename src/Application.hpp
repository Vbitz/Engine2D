#pragma once

#include "common.hpp"

#include "ScriptingManager.hpp"

#include "ResourceManager.hpp"

#include "vendor/GLFT_Font.hpp"

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
        
        bool RunFile(std::string path, bool persist);
        
        void Exit();
        void ToggleFullscreen();
        void SaveScreenshot(std::string filename);
        void DumpProfile();
        void RestartRenderer();
        
        bool UsingGL3();
        
        void RunCommand(std::string str);
        
        void DetailProfile(int frames, std::string filename);
        
        void InvalidateScript(std::string filename);
        
        bool GetKeyPressed(int key);
        
        OpenGLVersion GetOpenGLVersion();
        
        EffectShaderTypes::Type GetBestEffectShaderType();
        
        static std::string GetEngineVersion();
        
        void UpdateScreen();
        
    private:
        void _mainLoop();
        
        int  _postStart();
        void _dumpProfile();
        void _saveScreenshot();
        void _toggleFullscreen();
        void _restartRenderer();
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
        void _disablePreload();
        void _updateFrameTime();
        void _updateMousePos();
        void _shutdownScripting();
        void _printConfigVars();
        void _loadConfigFile();
        void _enableV8Debugger();
        void _handleDebugMessage();
        v8::Handle<v8::Context> _initScripting();
        void _enableTypedArrays();
        void _initGLContext(GraphicsVersion v);
        
        static void _rendererKillHandler(Json::Value v);
        static void _postCreateContext(Json::Value v);
        
        bool _running = false;
        
        bool _toggleNextframe = false;
        bool _screenshotNextframe = false;
        bool _dumpProfileAtFrameEnd = false;
        bool _restartNextFrame = false;
        
        std::string _screenshotFilename;
        
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
    };
    
    Application* GetAppSingilton();
}