/*
   Filename: Application.cpp
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

#define GLEW_STATIC
#include "vendor/GL/glew.h"

#include "Application.hpp"

#include <cstring>

#include "vendor/json/json.h"
#include "vendor/soil/SOIL.h"

#include "Config.hpp"
#include "Profiler.hpp"

#include "ResourceManager.hpp"

#include "TestSuite.hpp"

#include "FramePerfMonitor.hpp"
#include "Timer.hpp"

#include "PlatformTests.hpp"
#include "CoreTests.hpp"
#include "ScriptingTests.hpp"
#include "StdLibTests.hpp"
#include "PackageTests.hpp"

#include "EngineUI.hpp"

#include <pthread.h>

#include "Build.hpp"

namespace Engine {
    
    bool _hasGLContext = false;
    
    bool HasGLContext() {
        return _hasGLContext;
    }
    
    void EnableGLContext() {
        _hasGLContext = true;
    }
    
    void DisableGLContext() {
        _hasGLContext = false;
    }
    
    void DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, void* userParam) {
        Logger::begin("OpenGL", Logger::LogLevel_Error) << source << " : " << type << " : " <<
        id << " : " << severity << " : " << message << Logger::end();
    }
	
	void Application::_updateMousePos() {
        glm::vec2 mouse = _window->GetCursorPos();
        
        v8::Isolate* isolate = this->_scripting->GetIsolate();
        v8::Local<v8::Object> input_table = this->_scripting->GetScriptTable("input");
        
		input_table->Set(v8::String::NewFromUtf8(isolate, "mouseX"), v8::Number::New(isolate, mouse.x));
		input_table->Set(v8::String::NewFromUtf8(isolate, "mouseY"), v8::Number::New(isolate, mouse.y));
		input_table->Set(v8::String::NewFromUtf8(isolate, "leftMouseButton"), v8::Boolean::New(isolate, this->_window->GetMouseButtonPressed(MouseButton_Left)));
		input_table->Set(v8::String::NewFromUtf8(isolate, "middleMouseButton"), v8::Boolean::New(isolate, this->_window->GetMouseButtonPressed(MouseButton_Middle)));
		input_table->Set(v8::String::NewFromUtf8(isolate, "rightMouseButton"), v8::Boolean::New(isolate, this->_window->GetMouseButtonPressed(MouseButton_Right)));
	}
	
	void Application::UpdateScreen() {
        if (!this->_running) {
            return false;
        }
        
        v8::Isolate* isolate = this->_scripting->GetIsolate();
        v8::Local<v8::Object> sys_table = this->_scripting->GetScriptTable("sys");
        
        glm::vec2 size = this->_window->GetWindowSize();
        
		sys_table->Set(v8::String::NewFromUtf8(isolate, "screenWidth"), v8::Number::New(isolate, size.x));
		sys_table->Set(v8::String::NewFromUtf8(isolate, "screenHeight"), v8::Number::New(isolate, size.y));
	}
    
    void Application::_updateFrameTime() {
        v8::Isolate* isolate = this->_scripting->GetIsolate();
		v8::Local<v8::Object> sys_table = this->_scripting->GetScriptTable("sys");
        
        sys_table->Set(v8::String::NewFromUtf8(isolate, "deltaTime"), v8::Number::New(isolate, FramePerfMonitor::GetFrameTime()));
    }
    
    void Application::_disablePreload() {
        v8::Isolate* isolate = this->_scripting->GetIsolate();
        v8::HandleScope scope(isolate);
        
        this->_scripting->GetScriptTable("draw")->SetHiddenValue(v8::String::NewFromUtf8(isolate, "_draw"), v8::External::New(isolate, new Draw2D(GetRender())));
        
        this->_scripting->GetScriptTable("sys")->SetHiddenValue(v8::String::NewFromUtf8(isolate, "_app"), v8::External::New(isolate, this));
        
        this->_scripting->GetScriptTable("sys")->Set(v8::String::NewFromUtf8(isolate, "preload"), v8::Boolean::New(isolate, false));
    }
    
    void Application::_loadBasicConfigs() {
        // new names in comments
        // I still need to think though and make sure these match up
        
        Config::SetBoolean( "core.runOnIdle",                       false);
        Config::SetBoolean( "core.throttleOnIdle",                  true);
        Config::SetBoolean( "core.catchErrors",                     !this->_debugMode && !this->_testMode);
        
        Config::SetBoolean( "core.debugMode",                       this->_debugMode);
        Config::SetBoolean( "core.devMode",                         this->_developerMode);
        Config::SetBoolean( "core.testMode",                        this->_testMode);
        
        Config::SetNumber(  "core.window.width",                    800);
        Config::SetNumber(  "core.window.height",                   600);
        Config::SetNumber(  "core.render.aa",                       4);
        Config::SetBoolean( "core.window.vsync",                    false); // lack of vsync causes FPS issues
        Config::SetBoolean( "core.window.fullscreen",               false);
        Config::SetString(  "core.render.openGL",                   "3.2");
        Config::SetString(  "core.content.fontPath",                "fonts/open_sans.json");
        Config::SetBoolean( "core.debug.engineUI.showVerboseLog",   false);
        Config::SetBoolean( "core.debug.engineUI",                  this->_developerMode);
        Config::SetNumber( "core.debug.engineUI.profilerScale",     500);
        Config::SetBoolean( "core.debug.profiler",                  this->_developerMode || this->_debugMode);
        Config::SetString(  "core.window.title",                    "Engine2D");
        Config::SetBoolean( "core.debug.debugRenderer",             false);
        Config::SetString(  "core.render.basicEffect",              "shaders/basic.json");
        Config::SetNumber(  "core.render.targetFrameTime",          1.0f / 30.0f);
        
        Config::SetBoolean( "core.render.clampTexture",             true);
        Config::SetBoolean( "core.render.forceMipmaps",             true);
        
        Config::SetBoolean( "core.script.autoReload",               this->_developerMode);
        Config::SetBoolean( "core.script.gcOnFrame",                this->_developerMode);
        Config::SetString(  "core.script.loader",                   "lib/boot.js");
        Config::SetString(  "core.config.path",                     "config/config.json");
        Config::SetString(  "core.script.entryPoint",               "script/basic");
        
        // log_console = core.log.enableConsole
        // With quite a bit of research into console logging performance on windows it seems like I should be using
        // printf or buffered std::cout
        Config::SetBoolean( "core.log.enableConsole",               true);
        Config::SetBoolean( "core.log.filePath",                    "");
        Config::SetBoolean( "core.log.levels.verbose",              this->_developerMode || this->_debugMode);
        Config::SetBoolean("core.log.levels.onlyHighlight",         false);
        Config::SetBoolean( "core.log.showColors",                  true);
        Config::SetBoolean( "core.log.src.undefinedValue",          this->_developerMode);
        Config::SetBoolean( "core.log.src.perfIssues",              this->_developerMode);
        Config::SetBoolean( "core.log.src.createImage",             true);
        
        Config::SetBoolean( "core.debug.v8Debug",                   this->_developerMode);
        Config::SetNumber(  "core.debug.v8Debug.port",              5858);
        Config::SetBoolean( "core.debug.slowload",                  false);
        Config::SetBoolean( "core.render.halfPix",                  false);
        Config::SetNumber(  "core.test.testFrames",                 0);
        Config::SetNumber(  "core.test.screenshotTime",             0);
    }
    
    EventMagic Application::_config_CoreRenderAA(Json::Value args) {
        GetAppSingilton()->GetWindow()->SetAntiAlias(Config::GetInt("core.render.aa"));
        return EM_OK;
    }
    
    EventMagic Application::_config_CoreWindowVSync(Json::Value args) {
        GetAppSingilton()->GetWindow()->SetVSync(Config::GetBoolean("core.window.vsync"));
        return EM_OK;
    }
    
    EventMagic Application::_config_CoreWindowSize(Json::Value args) {
        GetAppSingilton()->GetWindow()->SetWindowSize(glm::vec2(Config::GetInt("core.window.width"), Config::GetInt("core.window.height")));
        return EM_OK;
    }
    
    EventMagic Application::_config_CoreWindowTitle(Json::Value args) {
        GetAppSingilton()->GetWindow()->SetTitle(Config::GetString("core.window.title"));
        return EM_OK;
    }
    
    void Application::_hookConfigs() {
        GetEventsSingilton()->GetEvent("config:core.render.aa")->AddListener("Application::Config_CoreRenderAA", EventEmitter::MakeTarget(_config_CoreRenderAA));
        GetEventsSingilton()->GetEvent("config:core.window.vsync")->AddListener("Application::Config_CoreWindowVSync", EventEmitter::MakeTarget(_config_CoreWindowVSync));
        GetEventsSingilton()->GetEvent("config:core.window.width")->AddListener("Application::ConfigWindowSize_Width", EventEmitter::MakeTarget(_config_CoreWindowSize));
        GetEventsSingilton()->GetEvent("config:core.window.height")-> AddListener("Application::ConfigWindowSize_Height", EventEmitter::MakeTarget(_config_CoreWindowSize));
        GetEventsSingilton()->GetEvent("config:core.window.title")->AddListener("Application::Config_CoreWindowTitle", EventEmitter::MakeTarget(_config_CoreWindowTitle));
    }
    
    void Application::_loadConfigFile(std::string configPath) {
        Logger::begin("Application", Logger::LogLevel_Log) << "Loading Config: " << configPath << Logger::end();
        Json::Value root;
        Json::Reader reader;
        bool fileLoaded = reader.parse(Filesystem::GetFileContent(configPath), root);
        if (!fileLoaded) {
            Logger::begin("Application", Logger::LogLevel_Error) << "Failed to load Config" << Logger::end();
            return;
        }
        if (!root.isObject()) {
            Logger::begin("Application", Logger::LogLevel_Error) << "Config root has to be a object" << Logger::end();
            return;
        }
        if (root.empty()) {
            return;
        }
        Json::Value::Members keys = root.getMemberNames();
        for (auto iter = keys.begin(); iter != keys.end(); iter++) {
            Json::Value value = root[*iter];
            if (value.isNull()) {
                // ignore null values
            } else if (value.isString()) {
                Config::SetString(*iter, value.asString());
            } else if (value.isNumeric()) {
                Config::SetNumber(*iter, value.asFloat());
            } else if (value.isBool()) {
                Config::SetBoolean(*iter, value.asBool());
            }
        }
    }
    
    void Application::_printConfigVars() {
        Logger::begin("Application", Logger::LogLevel_Log) << "== Config Vars ==" << Logger::end();
        std::vector<std::string> items = Config::GetAll();
        for (auto iter = items.begin(); iter != items.end(); iter++) {
            Logger::begin("Config", Logger::LogLevel_Log) << *iter << Logger::end();
        }
    }
    
    EventMagic Application::_appEvent_Exit(Json::Value val) {
        GetAppSingilton()->Exit();
        return EM_OK;
    }
    
    EventMagic Application::_appEvent_DumpScripts(Json::Value args) {
        GetAppSingilton()->DumpScripts();
        return EM_OK;
    }
    
    void Application::_hookEvents() {
        GetEventsSingilton()->GetEvent("exit")->AddListener("Application::_appEvent_Exit", EventEmitter::MakeTarget(_appEvent_Exit));
        GetEventsSingilton()->GetEvent("dumpScripts")->AddListener("Applicaton::_appEvent_DumpScripts", EventEmitter::MakeTarget(_appEvent_DumpScripts));
        
        GetEventsSingilton()->GetEvent("toggleFullscreen")->AddListener("Application::_toggleFullscreen", EventEmitter::MakeTarget(_toggleFullscreen))->SetDefered(true);
        GetEventsSingilton()->GetEvent("restartRenderer")->AddListener("Application::_restartRenderer", EventEmitter::MakeTarget(_restartRenderer))->SetDefered(true);
        GetEventsSingilton()->GetEvent("screenshot")->AddListener("Application::_saveScreenshot", EventEmitter::MakeTarget(_saveScreenshot))->SetDefered(true);
        GetEventsSingilton()->GetEvent("dumpLog")->AddListener("Application::_dumpLog", EventEmitter::MakeTarget(_dumpLog));
        
        GetEventsSingilton()->GetEvent("runFile")->AddListener(10, "Application::_requireDynamicLibary", EventEmitter::MakeTarget(_requireDynamicLibary));
        GetEventsSingilton()->GetEvent("runFile")->AddListener(10, "Application::_requireConfigFile", EventEmitter::MakeTarget(_requireConfigFile));
    }
	
    void _resizeWindow(Json::Value val) {
        int w = val["width"].asInt(),
            h = val["height"].asInt();
        ApplicationPtr app = GetAppSingilton();
        Logger::begin("Window", Logger::LogLevel_Verbose) << "Resizing Window to " << w << "x" << h << Logger::end();
		app->UpdateScreen();
        glViewport(0, 0, w, h);
        app->GetRender()->CheckError("Post Viewpoint");
    }
    
    void Application::_initGLContext(GraphicsVersion v) {
        this->_renderGL = this->_window->GetRender();
        
        glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
        
        if (v == GraphicsVersion::OpenGL_Legacy) {
            glDisable(GL_LIGHTING);
        }
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        if (Config::GetBoolean("core.debug.debugRenderer") && glDebugMessageCallback != NULL) {
            glDebugMessageCallback(DebugMessageCallback, NULL);
        }
        
        this->GetRender()->CheckError("PostSetupContext");
        
        Logger::begin("Window", Logger::LogLevel_Highlight) << "Loaded OpenGL" << Logger::end();

        this->GetRender()->Init2d();
    }
    
    void Application::_openWindow(int width, int height, bool fullscreen, std::string openGL3Context) {
        Logger::begin("Window", Logger::LogLevel_Log) << "Loading OpenGL : Init Window/Context" << Logger::end();
        
        GraphicsVersion v;
        
        if (openGL3Context == "3.2") {
            v = GraphicsVersion::OpenGL_Modern;
        } else {
            v = GraphicsVersion::OpenGL_Legacy;
        }
        
        this->_window = CreateWindow(v);
        
        this->_window->SetWindowSize(glm::vec2(width, height));
        this->_window->SetTitle(Config::GetString("core.window.title"));
        this->_window->SetFullscreen(fullscreen);
        this->_window->SetAntiAlias(Config::GetInt("core.render.aa"));
        this->_window->SetDebug(Config::GetBoolean("core.debug.debugRenderer"));
        
        this->_window->Show();
        
        Logger::begin("Window", Logger::LogLevel_Verbose) << "Loading OpenGL : Init OpenGL State" << Logger::end();
        
        this->GetRender()->CheckError("PostSetCallback");
    }
    
    void Application::_closeWindow() {
        Logger::begin("Window", Logger::LogLevel_Log) << "Terminating Window" << Logger::end();
        delete this->_window;
        this->_window = NULL;
    }
    
    EventMagic Application::_rawInputHandler(Json::Value val) {
        ApplicationPtr app = GetAppSingilton();
        
        app->_engineUI->OnKeyPress(val["rawKey"].asInt(), val["rawPress"].asInt(), val["shift"].asBool());
        
        if (app->_engineUI->ConsoleActive()) {
            return;
        }
        
        std::string key = val["key"].asString();
        
        GetEventsSingilton()->GetEvent("key_" + key)->Emit(val);
        GetEventsSingilton()->GetEvent("input")->Emit(val);
        
        return EM_OK;
    }
    
    EventMagic Application::_rendererKillHandler(Json::Value val) {
        Logger::begin("Window", Logger::LogLevel_Verbose) << "Window Destroyed" << Logger::end();
        ResourceManager::UnloadAll();
        return EM_OK;
    }
    
    EventMagic Application::_postCreateContext(Json::Value val) {
        ApplicationPtr app = GetAppSingilton();
        app->_initGLContext(app->_window->GetGraphicsVersion());
        return EM_OK;
    }
    
    EventMagic Application::_rawResizeHandler(Json::Value val) {
        GetAppSingilton()->UpdateScreen();
        return EM_OK;
    }
	
	void Application::_initOpenGL() {
        Logger::begin("Window", Logger::LogLevel_Verbose) << "Loading OpenGL : Init Window" << Logger::end();
        
        GetEventsSingilton()->GetEvent("destroyWindow")->AddListener("Application::RendererKillHandler", EventEmitter::MakeTarget(_rendererKillHandler));
        GetEventsSingilton()->GetEvent("rawInput")->AddListener("Application::RawInputHandler", EventEmitter::MakeTarget(_rawInputHandler));
        GetEventsSingilton()->GetEvent("postCreateContext")->AddListener("Application::_postCreateContext", EventEmitter::MakeTarget(_postCreateContext));
        GetEventsSingilton()->GetEvent("rawResize")->AddListener("Application::RawResizeHandler", EventEmitter::MakeTarget(_rawResizeHandler));
        
        Window::StaticInit();
        
        this->_openWindow(Config::GetInt("core.window.width"), Config::GetInt("core.window.height"),
                   Config::GetBoolean("core.window.fullscreen"), Config::GetString("core.render.openGL"));
	}
	
	void Application::_shutdownOpenGL() {
        this->_closeWindow();
        Window::StaticDestroy();
	}
    
    // command line handlers
    
    void Application::_applyDelayedConfigs() {
        for (auto iter = _delayedConfigs.begin(); iter != _delayedConfigs.end(); iter++) {
            if (!Config::Set(iter->first, iter->second)) {
                Logger::begin("CommandLine", Logger::LogLevel_Error) << "Could not set '"
                << iter->first << "' to '" << iter->second
                << "' Ignoring" << Logger::end();
            }
        }
    }
    
    // command line parsing
    
    bool Application::_parseCommandLine(int argc, const char* argv[]) {
        bool _exit = false;
        
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-devmode") == 0) {
                // enable devmode
                _developerMode = true;
            } else if (strcmp(argv[i], "-debug") == 0) {
                // enable debug
                _debugMode = true;
            } else if (strcmp(argv[i], "-test") == 0) {
                // start test mode
                _testMode = true;
            } else if (strcmp(argv[i], "-headless") == 0) {
                // enable headless mode
            } else if (strcmp(argv[i], "-v8-options") == 0) {
                ScriptingManager::Context::RunHelpCommand();
                _exit = true; // v8 exits automaticly but let's just help it along
            } else if (strcmp(argv[i], "-Cvars") == 0) {
                _configVarsMode = true;
            } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-help") == 0) {
                std::cerr << "Engine2D\n"
                "==================\n"
                "\n"
                "-- Examples --\n"
                "bin/Engine - loads config/config.json and sets basic config values\n"
                "bin/Engine -Ccore.window.width=1280 -Ccore.window.height=800 - loads config/config.json and "
                "overrides the basic configs core.window.width and core.window.height\n"
                "bin/Engine -config=config/test.json - loads config/test.json and sets basic config values\n"
                "bin/Engine script/test - any non `-` configs are passed onto javascript\n"
                "\n"
                "-- Args --\n"
                "-Cname=value                   - Overloads a basic config. This is applyed before loading the basic config"
                " but overrides those configs while they are applyed.\n"
                "-Cvars                         - Prints all Config Variables megred from the defaults and the set config file.\n"
                "-mountPath=archiveFile         - Loads a archive file using PhysFS, this is applyed after physfs is started.\n"
                "-test                          - Runs the built in test suite.\n"
                "(NYI) -headless                - Loads scripting without creating a OpenGL context, any calls requiring OpenGL"
                " will fail.\n"
                "-devmode                       - Enables developer mode (This enables real time script loading and the console).\n"
                "-debug                         - Enables debug mode (This enables a OpenGL debug context and will print messages"
                "to the console).\n"
                "(NYI) -log=filename            - Logs logger output to filename (This is not writen using PhysFS so it needs a "
                "regular path).\n"
                "-v8-options                    - Prints v8 option help then exits.\n"
                "-v8flag=value                  - Set's v8 flags from the command line.\n"
                "-h                             - Prints this message.\n"
                "" << std::endl;
                _exit = true;
            } else if (strncmp(argv[i], "-C", 2) == 0) {
                // set delayed config
                size_t keyLength = strcspn(argv[i], "=") - 2;
                size_t valueLength = strlen(argv[i]) - keyLength - 3; // 2 chars for -C and 1 for =
                
                char* key = (char*) malloc(keyLength + 1);
                key[keyLength] = '\0';
                strncpy(key, &argv[i][2], keyLength);
                
                char* value = (char*) malloc(valueLength + 1);
                value[valueLength] = '\0';
                strncpy(value, &argv[i][2 + keyLength + 1], valueLength);
                
                _delayedConfigs[key] = std::string(value);
                
                free(key);
                free(value);
            } else if (strncmp(argv[i], "-v8", 3) == 0) {
                // set delayed config
                size_t configLength = strlen(argv[i]) - 3;
                char* key = (char*) malloc(2 + configLength + 1);
                key[0] = '-'; key[1] = '-';
                strncpy(&key[2], &argv[i][3], configLength);
                key[2 + configLength] = '\0';
                
                Logger::begin("Scripting_CFG", Logger::LogLevel_Log) << "Setting V8 Option: " << key << Logger::end();
                
                ScriptingManager::Context::SetFlag(std::string(key));
                
                free(key);
            } else if (strncmp(argv[i], "-mountPath=", 11) == 0) {
                // add archive to PhysFS after PhysFS Init
                size_t configLength = strlen(argv[i]) - 11;
                char* archive = (char*) malloc(configLength + 1);
                strncpy(archive, &argv[i][11], configLength);
                archive[configLength] = '\0';
                
                this->_archivePaths.push_back(archive);
                
                free(archive);
            } else if (strncmp(argv[i], "-log=", 5) == 0) {
                // set logfile and enable logging
            } else {
                // push to JS args
                _jsArgs.push_back(argv[i]);
            }
        }
        
        return !_exit;
    }
    
    // Test Suite Loading
	
    void Application::_loadTests() {
        if (_debugMode) {
            TestSuite::LoadTestSuiteTests();
        }
        LoadPlatformTests();
        LoadCoreTests();
        LoadScriptingTests();
        LoadStdLibTests();
        LoadPackageTests();
    }
	
	// public methods
    
    bool Application::IsDelayedConfig(std::string configKey) {
        return this->_delayedConfigs.count(configKey) != 0;
    }
    
    std::vector<std::string> Application::GetCommandLineArgs() {
        return _jsArgs;
    }
    
    void Application::Exit() {
        this->_running = false;
    }
    
    EventMagic Application::_restartRenderer(Json::Value args) {
        ApplicationPtr app = GetAppSingilton();
        Logger::begin("Window", Logger::LogLevel_Log) << "Restarting renderer" << Logger::end();
        app->_window->Reset();
        app->_initGLContext(app->_window->GetGraphicsVersion());
        app->UpdateScreen();
    }
    
    EventMagic Application::_toggleFullscreen(Json::Value args) {
        ApplicationPtr app = GetAppSingilton();
        app->_window->SetFullscreen(!app->_window->GetFullscreen());
        app->UpdateScreen();
    }
    
    EventMagic Application::_saveScreenshot(Json::Value args) {
        ApplicationPtr app = GetAppSingilton();
        std::string targetFilename = args["filename"].asString();
        
        int width, height;
        
        width = app->_window->GetWindowSize().x;
        height = app->_window->GetWindowSize().y;
        
        Filesystem::TouchFile(targetFilename);
        
        SOIL_save_screenshot(Filesystem::GetRealPath(targetFilename).c_str(), SOIL_SAVE_TYPE_BMP, 0, 0, width, height);
        
        Logger::begin("Screenshot", Logger::LogLevel_Log) << "Saved Screenshot as: " << Filesystem::GetRealPath(targetFilename) << Logger::end();
        
        Json::Value saveArgs(Json::objectValue);
        
        saveArgs["filename"] = Filesystem::GetRealPath(targetFilename);
        
        GetEventsSingilton()->GetEvent("onSaveScreenshot")->Emit(saveArgs);
    }
    
    EventMagic Application::_dumpLog(Json::Value args) {
        std::string targetFilename = args["filename"].asString();
        std::string log = Logger::DumpAllEvents();
        
        Filesystem::WriteFile(targetFilename, log.c_str(), log.length());
        
        Logger::begin("DumpLog", Logger::LogLevel_Log) << "Saved Logfile as: " << Filesystem::GetRealPath(targetFilename) << Logger::end();
        
        Json::Value saveArgs(Json::objectValue);
        
        saveArgs["filename"] = Filesystem::GetRealPath(targetFilename);
        
        GetEventsSingilton()->GetEvent("onSaveLog")->Emit(saveArgs);
    }
    
    EventMagic Application::_requireDynamicLibary(Json::Value args) {
        static size_t endingLength = strlen(_PLATFORM_DYLINK);
        std::string filename = args["path"].asString();
        if (0 == filename.compare(filename.length() - endingLength, endingLength, _PLATFORM_DYLINK)) {
            Logger::begin("Application::_requireDynamicLibary", Logger::LogLevel_Verbose) << "Loading shared library: " << filename << Logger::end();
            Platform::LibaryPtr lib = Platform::OpenLibary(Filesystem::GetRealPath(filename));
            if (!lib->IsValid()) {
                Logger::begin("Application::_requireDynamicLibary", Logger::LogLevel_Error) << "Error loading shared library: " << filename << Logger::end();
            }
            return EM_CANCEL;
        } else {
            return EM_OK;
        }
    }
    
    EventMagic Application::_requireConfigFile(Json::Value args) {
        static size_t endingLength = strlen(".json");
        std::string filename = args["path"].asString();
        if (0 == filename.compare(filename.length() - endingLength, endingLength, ".json")) {
            GetAppSingilton()->_loadConfigFile(filename);
            return EM_CANCEL;
        } else {
            return EM_OK;
        }
    }
    
    bool Application::GetKeyPressed(int key) {
        return this->_window->GetKeyStatus(key) == Key_Press;
    }
    
    std::string Application::GetEngineVersion() {
        return std::string("Engine2D v ") + std::string(ENGINE_VERSION);
    }
    
    bool Application::IsDebugMode() {
        return this->_debugMode;
    }
    
    bool Application::IsDeveloperMode() {
        return this->_developerMode;
    }
    
    void Application::Assert(bool value, std::string reason, std::string line, int lineNumber) {
        if (value || !this->_debugMode) return;
        Logger::begin("Assert", Logger::LogLevel_Error) << "ASSERT FAILED : (" << line << ":" << lineNumber << ") " << reason << Logger::end();
        Platform::DumpStackTrace();
        throw "Assert Failed";
        //exit(EXIT_FAILURE); // Can't recover from this
    }
    
    void Application::AddScript(const char* filename_str, size_t filename_len) {
        this->_pendingScripts.push({
            .filename_str = filename_str,
            .filename_len = filename_len
        });
    }
    
    void Application::DumpScripts() {
        Logger::begin("Application::DumpScripts", Logger::LogLevel_Log) << "Starting Application::DumpScripts" << Logger::end();
        for (auto iter = this->_scripts.begin(); iter != this->_scripts.end(); iter++) {
            Logger::begin("Application::DumpScripts", Logger::LogLevel_Log) << "    " << iter->second.filename << Logger::end();
        }
    }
    
    // Modifyed slightly from node.js at https://github.com/joyent/node/blob/master/src/node_win32_etw_provider.cc
    
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
    
    struct v8tags {
        char prefix[32 - sizeof(size_t)];
        size_t prelen;
    };
    
    // The v8 CODE_ADDED event name has a prefix indicating the type of event.
    // Many of these are internal to v8.
    // The trace_codes array specifies which types are written.
    struct v8tags trace_codes[] = {
#define MAKE_V8TAG(s) { s, sizeof(s) - 1 }
        MAKE_V8TAG("LazyCompile:"),
        MAKE_V8TAG("Script:"),
        MAKE_V8TAG("Function:"),
        MAKE_V8TAG("RegExp:"),
        MAKE_V8TAG("Eval:")
#undef MAKE_V8TAG
    };
    
    // v8 sometimes puts a '*' or '~' in front of the name.
#define V8_MARKER1 '*'
#define V8_MARKER2 '~'
    
    
    // If prefix is not in filtered list return -1,
    // else return length of prefix and marker.
    int _filterCodeEvents(const char* name, size_t len) {
        for (int i = 0; i < ARRAY_SIZE(trace_codes); i++) {
            size_t prelen = trace_codes[i].prelen;
            if (prelen < len) {
                if (strncmp(name, trace_codes[i].prefix, prelen) == 0) {
                    if (name[prelen] == V8_MARKER1 || name[prelen] == V8_MARKER2 || name[prelen] == ' ')
                        prelen++;
                    return prelen;
                }
            }
        }
        return -1;
    }

    
    void Application::_processScripts() {
        while (this->_pendingScripts.size() > 0) {
            RawScriptInfo i = this->_pendingScripts.front();
            
            int prefixLength = _filterCodeEvents(i.filename_str, i.filename_len);
            if (prefixLength >= 0) {
                std::string filename = std::string(i.filename_str + prefixLength, i.filename_len - prefixLength);
                if (filename.length() > 0) {
                    if (this->_scripts.count(filename) > 0) {
                        // update infomation
                    } else {
                        // add new
                        this->_scripts[filename] = {
                            .filename = filename
                        };
                    }
                }
            }
            
            this->_pendingScripts.pop();
        }
    }
    
    void Application::_updateAddonLoad(LoadOrder load) {
        Addon::LoadAll(load);
        this->_currentLoadingState = load;
    }
    
    void Application::_mainLoop() {
        this->_running = true;
        
		while (this->_running) {
            if (!this->_window->ShouldClose() &&  // Check to make sure were not going to close
                !this->_window->IsFocused()) { // Check to make sure were not focused
                if (!this->_window->GetFullscreen() && // Check to make sure were not in fullscreen mode
                    !Config::GetBoolean("core.runOnIdle")) {
                    double startPauseTime = Platform::GetTime();
                    this->_window->WaitEvents();
                    Platform::Sleep(0);
                    // notify the timer that it needs to offset the values to keep time acurate for user interation and physics
                    Timer::NotifyPause(Platform::GetTime() - startPauseTime);
                    continue;
                } else {
                    if (Config::GetBoolean("core.throttleOnIdle") && !this->_window->GetFullscreen()) {
                        Platform::NanoSleep(150000);
                    }
                }
            }
            
            Profiler_New::BeginProfileFrame();
            FramePerfMonitor::BeginFrame();
            Timer::Update(); // Timer events may be emited now, this is the soonest into the frame that Javascript can run
            GetEventsSingilton()->PollDeferedMessages(); // Events from other threads will run here by default, Javascript may run at this time
            this->_processScripts();
            
            this->_scripting->CheckUpdate();
            
            this->_updateFrameTime();
            
            this->_updateMousePos();
            
            this->GetRender()->CheckError("startOfRendering");
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            
            FramePerfMonitor::BeginDraw();
            
            this->GetRender()->Begin2d();
            
            v8::Handle<v8::Value> args[1] = {
                v8::Number::New(v8::Isolate::GetCurrent(), FramePerfMonitor::GetFrameTime())
            };
            GetEventsSingilton()->GetEvent("draw")->Emit(Json::nullValue, 1, args); // this is when most Javascript runs
            
            this->GetRender()->End2d();
            
            this->GetRender()->Begin2d();
            
            //this->_cubeTest->Draw();
            
            this->_engineUI->Draw();
            
            this->GetRender()->End2d();
            
            FramePerfMonitor::EndDraw();
            
            this->_window->Present();
            
			if (this->_window->ShouldClose()) {
				Logger::begin("Window", Logger::LogLevel_Log) << "Exiting" << Logger::end();
				this->_running = false;
				break;
			}
            
            this->GetRender()->CheckError("endOfRendering");
            
            if (Config::GetBoolean("core.script.gcOnFrame")) {
                v8::V8::IdleNotification();
            }
            
            GetEventsSingilton()->PollDeferedMessages("toggleFullscreen");
            GetEventsSingilton()->PollDeferedMessages("restartRenderer");
            GetEventsSingilton()->PollDeferedMessages("screenshot");
            GetEventsSingilton()->PollDeferedMessages("dumpProfile");
            
            if (this->_frames == 0) {
                this->_updateAddonLoad(LoadOrder::FirstFrame);
                if (!this->_testMode) this->_frames++;
            }
            
            if (this->_testMode && this->_frames++ > Config::GetInt("core.test.testFrames")) {
                this->Exit();
            }
            
            FramePerfMonitor::EndFrame();
            Profiler_New::EndProfileFrame();
        }
    }
    
	// main function
    
    int Application::_postStart() {
        Logger::begin("Application", Logger::LogLevel_Log) << "Starting: " << Application::GetEngineVersion() << Logger::end();
        
        if (this->_debugMode) {
            Logger::begin("Application", Logger::LogLevel_Warning) << "=== Debug Mode Active ===" << Logger::end();
        }
        
        if (this->_developerMode) {
            Logger::begin("Application", Logger::LogLevel_Warning) << "=== Developer Mode Active ===" << Logger::end();
        }
        
		Filesystem::Init(Platform::GetRawCommandLineArgV()[0]);
        
        Config::EnableConfigEvents();
        this->_hookEvents();
        // The events system is now ready
        
        for (auto iter = this->_archivePaths.begin(); iter != this->_archivePaths.end(); iter++) {
            Filesystem::Mount(*iter);
        }
        
        // The filesystem is now ready for reading files
        
        this->_loadConfigFile(Config::GetString("core.config.path"));
        
        this->_applyDelayedConfigs();
        
        // Configuration is now setup
        
        if (this->_configVarsMode) {
            this->_printConfigVars();
            Filesystem::Destroy();
            return 0;
        }
        
        this->_updateAddonLoad(LoadOrder::PreScript);
        
        this->_scripting = new ScriptingManager::Context();
        
        // Scripting has now initalized, Javascript may punch in during any event
        
        this->_updateAddonLoad(LoadOrder::PreGraphics);
        
        this->_initOpenGL();
        
        this->_engineUI = new EngineUI(this);
        
        // The window is now ready
        
        GetRender()->CheckError("Post InitOpenGL");
        
        Engine::EnableGLContext();
        
        this->_disablePreload();
        
        GetEventsSingilton()->GetEvent("postLoad")->Emit();
        
        GetRender()->CheckError("On JS Post Load");
        
        //this->_cubeTest = this->_renderGL->CreateDrawable<Drawables::CubeDrawableTest>();
        
        this->UpdateScreen();
        
        GetRender()->CheckError("Post Finish Loading");
        
        this->_updateAddonLoad(LoadOrder::Loaded);
        
        Logger::begin("Application", Logger::LogLevel_Highlight) << "Loaded" << Logger::end();
        
        this->_hookConfigs(); // this is the last stage of the boot process so previous code does'nt interfere.
        
        if (this->_testMode) {
            this->_loadTests();
            TestSuite::Run();
            if (Config::GetInt("core.test.testFrames") > 0) {
                this->_mainLoop();
            }
        } else {
            // Let's get this going
            this->_mainLoop();
        }
        
        Engine::DisableGLContext();
        
		this->_shutdownOpenGL();
        
        delete this->_scripting;
        
        Addon::Shutdown();
        
		Filesystem::Destroy();
        
		return 0;
    }
	
	int Application::Start(int argc, char const *argv[]) {
        // At this point Logger is not avalible
        Logger::Init();
        Config::SetBoolean("core.log.enableConsole", true); // make sure we can log to the console right from the start
        
        Platform::SetRawCommandLine(argc, argv);
        
        if (!this->_parseCommandLine(argc, argv)) {
            return 1;
        }
        
        // The applicaiton has now parsed the command line
        
        Config::SetBoolean("core.log.enableConsole", false);
        
        this->_loadBasicConfigs();
        
        // Run _postStart
        if (Config::GetBoolean("core.catchErrors")) {
            try {
                return this->_postStart();
            } catch (...) {
                Platform::ShowMessageBox("Engine2D", "Engine2D has crashed and will now close", true);
                return 1;
            }
        } else {
            return this->_postStart();
        }
    }
    
    int EngineMain(int argc, char const *argv[]) {
        return GetAppSingilton()->Start(argc, argv);
    }
    
    ApplicationPtr _singilton = NULL;
    
    ApplicationPtr GetAppSingilton() {
        if (_singilton == NULL) {
            _singilton = new Application();
        }
        return _singilton;
    }
	
} // namespace Engine
