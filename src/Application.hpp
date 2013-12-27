#pragma once

#include "common.hpp"

#include "extern/GLFT_Font.hpp"

#include "ResourceManager.hpp"

namespace Engine {
    
    typedef struct _openGLVersion {
        int major, minor, revision;
    } OpenGLVersion;
    
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
        
    private:
        void _mainLoop();
        
        static void _keyPress(GLFWwindow* window, int rawKey, int scanCode, int state, int mods);
        static void _resizeWindow(GLFWwindow* window, int w, int h);
        
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
        void _updateScreen();
        void _updateMousePos();
        void _shutdownScripting();
        v8::Handle<v8::Context> _initScripting();
        
        bool _running = false;
        
        bool _isFullscreen = false;
        
        bool _toggleNextframe = false;
        bool _screenshotNextframe = false;
        bool _dumpProfileAtFrameEnd = false;
        bool _restartNextFrame = false;
        
        std::string _screenshotFilename;
        
        int _screenWidth = 0;
        int _screenHeight = 0;
        
        std::vector<std::string> _jsArgs;
        
        bool isGL3Context;
        
        GLFWwindow* window = NULL;
        
        std::map<std::string, ResourceManager::FontResource*> _fonts;
        
        std::map<std::string, std::string> _delayedConfigs;
        
        std::map<std::string, long> _loadedFiles;
        
        bool _developerMode = false;
        bool _debugMode = false;
        bool _testMode = false;
        
        int _detailFrames = 0;
        std::string _detailFilename = "";
    };
    
    Application* GetAppSingilton();
}