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

#include "Application.hpp"

#include <cstring>

#include "vendor/OpenSans-Regular.ttf.hpp"

#include "vendor/json/json.h"

#include "ResourceManager.hpp"

#include "TestSuite.hpp"

#include "FramePerfMonitor.hpp"
#include "Timer.hpp"

#include "PlatformTests.hpp"
#include "CoreTests.hpp"
#include "ScriptingTests.hpp"
#include "StdLibTests.hpp"

#include "EngineUI.hpp"

#include "JSSys.hpp"
#include "JSDraw.hpp"
#include "JSInput.hpp"
#include "JSFS.hpp"
#include "JSDatabase.hpp"
#include "JSMod.hpp"
#include "JSUnsafe.hpp"
#include "JSMathHelper.hpp"

#include <pthread.h>

#include <v8-debug.h>

#include "Build.hpp"

class MallocArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
public:
    virtual void* Allocate(size_t length) { return malloc(length); }
    virtual void* AllocateUninitialized(size_t length) { return malloc(length); }
    virtual void Free(void* data) { free(data); }
    virtual void Free(void* data, size_t length) { free(data); }
};

namespace Engine {
    
    static pthread_mutex_t debugMesssageReadyMutex;
    static bool debugMessageReady = false;
    
    static v8::Persistent<v8::External> _EM_CANCEL;
    
    void OnGLFWError(int error, const char* msg) {
        Logger::begin("Window", Logger::LogLevel_Error) << "GLFW Error : " << error << " : " << msg << Logger::end();
    }
    
    void DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, void* userParam) {
        Logger::begin("OpenGL", Logger::LogLevel_Error) << source << " : " << type << " : " <<
        id << " : " << severity << " : " << message << Logger::end();
    }
    
    static void globalAccessor(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
        info.GetReturnValue().Set(info.Holder());
    }
    
    ENGINE_JS_METHOD(EventCallCallback) {
        ENGINE_JS_SCOPE_OPEN;
        
        Json::Value eventArgs;
        
        if (args.Length() == 0) {
            eventArgs = Json::Value(Json::objectValue);
        } else {
            eventArgs = ScriptingManager::ObjectToJson(args[0].As<v8::Object>());
        }
        
        v8::Handle<v8::String> eventName = args.Data().As<v8::String>();
        
        std::string eventNameStr = std::string(*v8::String::Utf8Value(eventName));
        
        Events::Emit(eventNameStr, eventArgs);
        
        ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
    }
    
    static void EventGetterCallback(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value>& info) {
        v8::HandleScope scp(info.GetIsolate());
        
        std::string nameStr = std::string(*v8::String::Utf8Value(name));
        
        if (nameStr == "EM_CANCEL") {
            info.GetReturnValue().Set(_EM_CANCEL);
        } else {
            v8::Handle<v8::FunctionTemplate> t = v8::FunctionTemplate::New();
        
            t->SetCallHandler(EventCallCallback, name);
        
            v8::Handle<v8::Function> func = t->GetFunction();
        
            info.GetReturnValue().Set(func);
        }
    }
    
    static void EventSetterCallback(v8::Local<v8::String> name, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<v8::Value>& info) {
        v8::HandleScope scp(info.GetIsolate());
        
        if (!val->IsFunction()) {
            ENGINE_THROW_ARGERROR("The value passed needs to be a function");
            return;
        }
        
        v8::Handle<v8::Function> func = val.As<v8::Function>();
        
        std::string hookName = std::string(*v8::String::Utf8Value(name));
        std::string hookId = std::string(*v8::String::Utf8Value(func->GetName()));
        
        if (hookId.length() < 1) {
            ENGINE_THROW_ARGERROR("The function passed needs to be named");
            return;
        }
        
        Events::On(hookName, hookId, func);
    }
    
    void Application::_enableTypedArrays() {
        v8::V8::SetArrayBufferAllocator(new MallocArrayBufferAllocator());
    }
    
    void Application::_enableHarmony() {
        const char* harmony = "-harmony";
        v8::V8::SetFlagsFromString(harmony, std::strlen(harmony));
    }
    
    void Application::_createEventMagic() {
        if (_EM_CANCEL.IsEmpty()) {
            _EM_CANCEL.Reset(v8::Isolate::GetCurrent(), v8::External::New(new EventMagic(EM_CANCEL)));
        }
    }
    
    static void JitEventCallback(const v8::JitCodeEvent* e) {
        if (e != NULL && e->type == v8::JitCodeEvent::CODE_ADDED) {
            GetAppSingilton()->AddScript(e->name.str, e->name.len);
        }
    }
	
#define addItem(table, js_name, funct) table->Set(js_name, v8::FunctionTemplate::New(funct))
    
    v8::Handle<v8::Context> Application::_initScripting() {
        Logger::begin("Scripting", Logger::LogLevel_Log) << "Loading Scripting" << Logger::end();
        
        this->_enableTypedArrays();
        this->_enableHarmony();
        this->_createEventMagic();
        
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        
        v8::V8::SetJitCodeEventHandler(v8::JitCodeEventOptions::kJitCodeEventDefault, JitEventCallback);
        
		v8::HandleScope handle_scope(isolate);
        
		v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
        
        global->SetAccessor(v8::String::NewSymbol("global"), globalAccessor);
        
        addItem(global, "assert", JsSys::Assert);
        
        // consoleTable
        v8::Handle<v8::ObjectTemplate> consoleTable = v8::ObjectTemplate::New();
        
        addItem(consoleTable, "_log", JsSys::Println);
        addItem(consoleTable, "clear", JsSys::ClearConsole);
        addItem(consoleTable, "toggle", JsSys::ToggleConsole);
        
        global->Set("console", consoleTable);
        
		// sysTable
		v8::Handle<v8::ObjectTemplate> sysTable = v8::ObjectTemplate::New();
        
        JsSys::InitSys(sysTable);
        
        sysTable->Set("platform", v8::String::New(_PLATFORM));
        sysTable->Set("devMode", v8::Boolean::New(this->_developerMode));
        sysTable->Set("preload", v8::Boolean::New(true));
        sysTable->Set("numProcessers", v8::Number::New(Platform::GetProcesserCount()));
        
        // depending on the runtime being used in the future this will be set to something unique per system
        // for example on steam it can be the friends name or SteamID
        sysTable->Set("username", v8::String::New(Platform::GetUsername().c_str()));
        
		global->Set("sys", sysTable);
        
        v8::Handle<v8::ObjectTemplate> eventTable = v8::ObjectTemplate::New();
        
        eventTable->SetNamedPropertyHandler(EventGetterCallback, EventSetterCallback);
        
        global->Set("event", eventTable);
        
        // drawTable
        v8::Handle<v8::ObjectTemplate> drawTable = v8::ObjectTemplate::New();
        
        JsDraw::InitDraw(drawTable);
        
		global->Set("draw", drawTable);
        
        // fsTable
        v8::Handle<v8::ObjectTemplate> fsTable = v8::ObjectTemplate::New();
        
        JsFS::InitFS(fsTable);
        
        global->Set("fs", fsTable);
        
        // dbTable
        v8::Handle<v8::ObjectTemplate> dbTable = v8::ObjectTemplate::New();
        
        JSDatabase::InitDatabase(dbTable);
        
        global->Set("db", dbTable);
        
        // inputTable
        v8::Handle<v8::ObjectTemplate> inputTable = v8::ObjectTemplate::New();
        
        JsInput::InitInput(inputTable);
        
		global->Set("input", inputTable);
        
        // moduleTable
        v8::Handle<v8::ObjectTemplate> moduleTable = v8::ObjectTemplate::New();
        
        JSMod::InitMod(moduleTable);
        
        global->Set("mod", moduleTable);
        
        // unsafeTable
        
        if (this->_developerMode) {
            v8::Handle<v8::ObjectTemplate> unsafeTable = v8::ObjectTemplate::New();
        
            JsUnsafe::InitUnsafe(unsafeTable);
        
            global->Set("unsafe", unsafeTable);
        }
        
        v8::Handle<v8::Context> ctx = v8::Context::New(isolate, NULL, global);
        
        ctx->Enter();
        
        JsMathHelper::InitMathHelper();
        
		if (!this->_runFile(Config::GetString("core.script.loader"), true)) {
            Logger::begin("Scripting", Logger::LogLevel_Error) << "Bootloader not found" << Logger::end();
            EngineUI::ToggleConsole(); // give them something to debug using
        }
        
        Logger::begin("Scripting", Logger::LogLevel_Log) << "Loaded Scripting" << Logger::end();
        
        return handle_scope.Close(ctx);
	}
	
#undef addItem
    
    // WARNING: This is executed on another thread, basicly everything right now is not threadsafe
    static void DispatchDebugMessages() {
        pthread_mutex_lock(&debugMesssageReadyMutex);
        debugMessageReady = true;
        pthread_mutex_unlock(&debugMesssageReadyMutex);
    }
    
    void Application::_handleDebugMessage() {
        v8::Debug::ProcessDebugMessages();
    }
    
    void Application::_enableV8Debugger() {
        int debugPort = Config::GetInt("core.debug.v8Debug.port");
        v8::Debug::SetDebugMessageDispatchHandler(DispatchDebugMessages);
        v8::Debug::EnableAgent("Engine2D", debugPort, false);
        Config::SetBoolean("core.runOnIdle", true);
        pthread_mutex_init(&debugMesssageReadyMutex, NULL);
        Logger::begin("Application", Logger::LogLevel_Log) << "Started V8 Debugger on 127.0.0.1:" << debugPort << Logger::end();
    }
	
	void Application::_shutdownScripting() {
		//_globalIsolate->Exit();
		//_globalIsolate->Dispose();
	}
    
    v8::Local<v8::Object> Application::_getScriptTable(std::string name) {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        
		v8::Local<v8::Object> obj = ctx->Global();
		return v8::Local<v8::Object>::Cast(obj->Get(v8::String::New(name.c_str())));
    }
	
	void Application::_updateMousePos() {
        glm::vec2 mouse = _window->GetCursorPos();
        
        v8::Local<v8::Object> input_table = _getScriptTable("input");
        
		input_table->Set(v8::String::New("mouseX"), v8::Number::New(mouse.x));
		input_table->Set(v8::String::New("mouseY"), v8::Number::New(mouse.y));
		input_table->Set(v8::String::New("leftMouseButton"), v8::Boolean::New(this->_window->GetMouseButtonPressed(MouseButton_Left)));
		input_table->Set(v8::String::New("middleMouseButton"), v8::Boolean::New(this->_window->GetMouseButtonPressed(MouseButton_Middle)));
		input_table->Set(v8::String::New("rightMouseButton"), v8::Boolean::New(this->_window->GetMouseButtonPressed(MouseButton_Right)));
	}
	
	void Application::UpdateScreen() {
        if (!this->_running) {
            return false;
        }
        
        v8::Local<v8::Object> sys_table = _getScriptTable("sys");
        
        glm::vec2 size = this->_window->GetWindowSize();
        
		sys_table->Set(v8::String::New("screenWidth"), v8::Number::New(size.x));
		sys_table->Set(v8::String::New("screenHeight"), v8::Number::New(size.y));
	}
    
    void Application::_updateFrameTime() {
		v8::Local<v8::Object> sys_table = _getScriptTable("sys");
        
        sys_table->Set(v8::String::NewSymbol("deltaTime"), v8::Number::New(FramePerfMonitor::GetFrameTime()));
    }
    
    void Application::_disablePreload() {
		v8::Local<v8::Object> sys_table = _getScriptTable("sys");
        
        sys_table->Set(v8::String::New("preload"), v8::Boolean::New(false));
    }
    
    void Application::_loadBasicConfigs() {
        // new names in comments
        // I still need to think though and make sure these match up
        
        Config::SetBoolean( "core.runOnIdle",                       false);
        Config::SetBoolean( "core.throttleOnIdle",                  true);
        Config::SetBoolean( "core.catchErrors",                     !this->_debugMode);
        
        Config::SetNumber(  "core.window.width",                    800);
        Config::SetNumber(  "core.window.height",                   600);
        Config::SetNumber(  "core.render.aa",                       4);
        Config::SetBoolean( "core.window.vsync",                    false); // lack of vsync causes FPS issues
        Config::SetBoolean( "core.window.fullscreen",               false);
        Config::SetString(  "core.render.openGL",                   "3.2");
        Config::SetString(  "core.content.fontPath",                "fonts/OpenSans-Regular.ttf");
        Config::SetBoolean( "core.debug.engineUI.showVerboseLog",   false);
        Config::SetBoolean( "core.debug.engineUI",                  this->_developerMode);
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
        Config::SetBoolean( "core.log.showColors",                  true);
        Config::SetBoolean( "core.log.src.undefinedValue",          this->_developerMode);
        Config::SetBoolean( "core.log.src.perfIssues",              this->_developerMode);
        Config::SetBoolean( "core.log.src.createImage",             true);
        
        Config::SetBoolean( "core.debug.v8Debug",                   this->_developerMode);
        Config::SetNumber(  "core.debug.v8Debug.port",              5858);
        Config::SetBoolean( "core.debug.slowload",                  false);
    }
    
    EventMagic Config_CoreRenderAA(Json::Value args) {
        GetAppSingilton()->GetWindow()->SetAntiAlias(Config::GetInt("core.render.aa"));
        return EM_OK;
    }
    
    EventMagic Config_CoreWindowVSync(Json::Value args) {
        GetAppSingilton()->GetWindow()->SetVSync(Config::GetBoolean("core.window.vsync"));
        return EM_OK;
    }
    
    EventMagic Config_CoreWindowSize(Json::Value args) {
        GetAppSingilton()->GetWindow()->SetWindowSize(glm::vec2(Config::GetInt("core.window.width"), Config::GetInt("core.window.height")));
        return EM_OK;
    }
    
    EventMagic Config_CoreWindowTitle(Json::Value args) {
        GetAppSingilton()->GetWindow()->SetTitle(Config::GetString("core.window.title"));
        return EM_OK;
    }
    
    void Application::_hookConfigs() {
        Events::On("config:core.render.aa", "Application::Config_CoreRenderAA", Config_CoreRenderAA);
        Events::On("config:core.window.vsync", "Application::Config_CoreWindowVSync", Config_CoreWindowVSync);
        Events::On("config:core.window.width", "Application::ConfigWindowSize_Width", Config_CoreWindowSize);
        Events::On("config:core.window.height", "Application::ConfigWindowSize_Height", Config_CoreWindowSize);
        Events::On("config:core.window.title", "Application::Config_CoreWindowTitle", Config_CoreWindowTitle);
    }
    
    void Application::_loadConfigFile() {
        std::string configPath = Config::GetString("core.config.path");
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
    
    EventMagic AppEvent_Exit(Json::Value val) {
        GetAppSingilton()->Exit();
        return EM_OK;
    }
    
    EventMagic AppEvent_Screenshot(Json::Value args) {
        GetAppSingilton()->SaveScreenshot(args["filename"].asString());
        return EM_OK;
    }
    
    EventMagic AppEvent_DumpScripts(Json::Value args) {
        GetAppSingilton()->DumpScripts();
        return EM_OK;
    }
    
    void Application::_hookEvents() {
        Events::On("exit", "Application::AppEvent_Exit", AppEvent_Exit);
        Events::On("screenshot", "Application::AppEvent_Screenshot", AppEvent_Screenshot);
        Events::On("dumpScripts", "Applicaton::AppEvent_DumpScripts", AppEvent_DumpScripts);
    }
	
    void _resizeWindow(Json::Value val) {
        int w = val["width"].asInt(),
            h = val["height"].asInt();
        Application* app = GetAppSingilton();
        Logger::begin("Window", Logger::LogLevel_Verbose) << "Resizing Window to " << w << "x" << h << Logger::end();
		app->UpdateScreen();
        glViewport(0, 0, w, h);
        Draw2D::CheckGLError("Post Viewpoint");
    }
    
    void Application::_initGLContext(GraphicsVersion v) {
        glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
        
        if (v == Graphics_OpenGL_Legacy) {
            glDisable(GL_LIGHTING);
        }
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        if (Config::GetBoolean("core.debug.debugRenderer") && glDebugMessageCallback != NULL) {
            glDebugMessageCallback(DebugMessageCallback, NULL);
        }
        
        Draw2D::CheckGLError("PostSetupContext");
        
        Logger::begin("Window", Logger::LogLevel_Log) << "Loaded OpenGL" << Logger::end();
        
        this->_initFonts();
        Draw2D::Init2d();
    }
    
    void Application::_openWindow(int width, int height, bool fullscreen, std::string openGL3Context) {
        Logger::begin("Window", Logger::LogLevel_Log) << "Loading OpenGL : Init Window/Context" << Logger::end();
        
        GraphicsVersion v;
        
        if (openGL3Context == "3.2") {
            v = Graphics_OpenGL_Modern;
        } else {
            v = Graphics_OpenGL_Legacy;
        }
        
        this->_window = CreateWindow(v);
        
        this->_window->SetWindowSize(glm::vec2(width, height));
        this->_window->SetTitle(Config::GetString("core.window.title"));
        this->_window->SetFullscreen(fullscreen);
        this->_window->SetAntiAlias(Config::GetInt("core.render.aa"));
        this->_window->SetDebug(Config::GetBoolean("core.debug.debugRenderer"));
        
        this->_window->Show();
        
        Logger::begin("Window", Logger::LogLevel_Verbose) << "Loading OpenGL : Init OpenGL State" << Logger::end();
        
        Draw2D::CheckGLError("PostSetCallback");
    }
    
    void Application::_closeWindow() {
        Logger::begin("Window", Logger::LogLevel_Log) << "Terminating Window" << Logger::end();
        delete this->_window;
        this->_window = NULL;
    }
    
    EventMagic Application::_rawInputHandler(Json::Value val) {
        EngineUI::OnKeyPress(val["rawKey"].asInt(), val["rawPress"].asInt(), val["shift"].asBool());
        
        if (EngineUI::ConsoleActive()) {
            return;
        }
        
        std::string key = val["key"].asString();
        
        Events::Emit("input", val);
        
        return EM_OK;
    }
    
    EventMagic Application::_rendererKillHandler(Json::Value val) {
        Logger::begin("Window", Logger::LogLevel_Verbose) << "Window Destroyed" << Logger::end();
        GetAppSingilton()->_shutdownFonts();
        ResourceManager::UnloadAll();
        return EM_OK;
    }
    
    EventMagic Application::_postCreateContext(Json::Value val) {
        Application* app = GetAppSingilton();
        app->_initGLContext(app->_window->GetGraphicsVersion());
        return EM_OK;
    }
    
    EventMagic Application::_rawResizeHandler(Json::Value val) {
        GetAppSingilton()->UpdateScreen();
        return EM_OK;
    }
	
	void Application::_initOpenGL() {
        Logger::begin("Window", Logger::LogLevel_Verbose) << "Loading OpenGL : Init GLFW" << Logger::end();
        
        Events::On("destroyWindow", "Application::RendererKillHandler", _rendererKillHandler);
        Events::On("rawInput", "Application::RawInputHandler", _rawInputHandler);
        Events::On("postCreateContext", "Application::_postCreateContext", _postCreateContext);
        Events::On("rawResize", "Application::RawResizeHandler", _rawResizeHandler);
        
        glfwSetErrorCallback(OnGLFWError);
        
		glfwInit();
        
        this->_openWindow(Config::GetInt("core.window.width"), Config::GetInt("core.window.height"),
                   Config::GetBoolean("core.window.fullscreen"), Config::GetString("core.render.openGL"));
	}
	
	void Application::_shutdownOpenGL() {
        this->_closeWindow();
		glfwTerminate();
	}
	
	// font rendering
    
	void Application::_initFonts() {
        Profiler::Begin("LoadFonts");
        if (!this->LoadFont("basic", Config::GetString("core.content.fontPath"))) {
            Logger::begin("Font", Logger::LogLevel_Warning) << "Font not found: " << Config::GetString("core.content.fontPath") << " falling back to inbuilt font" << Logger::end();
            ResourceManager::Load("basicFont", new ResourceManager::RawSource(OpenSans_Regular, sizeof(OpenSans_Regular)));
            _fonts["basic"] = new ResourceManager::FontResource("basicFont");
        }
        Profiler::End("LoadFonts");
	}
	
	void Application::_shutdownFonts() {
        _fonts.clear();
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
                v8::V8::SetFlagsFromString("--help", 6);
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
                
                char* key = (char*) malloc(keyLength);
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
                v8::V8::SetFlagsFromString(key, (int) strlen(key));
                
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
    }
    
	// semi-realtime time loading
	
	void Application::_checkUpdate() {
        if (Config::GetBoolean("core.script.autoReload")) {
            for(auto iterator = _loadedFiles.begin(); iterator != _loadedFiles.end(); iterator++) {
                long lastMod = Filesystem::GetFileModifyTime(iterator->first);
                if (lastMod > iterator->second) {
                    Json::Value args = Json::Value(Json::objectValue);
                    args["filename"] = iterator->first;
                    this->RunFile(iterator->first, true);
                    Events::Emit("scriptReloaded", args);
                }
            }
        } else {
            for(auto iterator = _loadedFiles.begin(); iterator != _loadedFiles.end(); iterator++) {
                if (iterator->second < 0) {
                    Json::Value args = Json::Value(Json::objectValue);
                    args["filename"] = iterator->first;
                    this->RunFile(iterator->first, true);
                    Events::Emit("scriptReloaded", args);
                }
            }
        }
	}
	
	// public methods
    
    bool Application::IsDelayedConfig(std::string configKey) {
        return this->_delayedConfigs.count(configKey) != 0;
    }
	
	int Application::GetScreenWidth() {
		return this->_window->GetWindowSize().x;
	}
	
	int Application::GetScreenHeight() {
		return this->_window->GetWindowSize().y;
	}

    void Application::SetScreenSize(int width, int height) {
        this->_window->SetWindowSize(glm::vec2(width, height));
    }
    
    Window* Application::GetWindow() {
        return this->_window;
    }
	
	GLFT_Font* Application::GetFont(std::string fontName, int size) {
        return _fonts[fontName]->GetFont(size);
	}
    
    
    bool Application::LoadFont(std::string prettyName, std::string filename) {
        Logger::begin("Font", Logger::LogLevel_Verbose)
            << "Loading Font: " << filename << " as " << prettyName
            << Logger::end();
        
        Draw2D::CheckGLError("PreLoadFont");
        
		if (Filesystem::FileExists(filename)) {
            ResourceManager::Load(filename);
            _fonts[prettyName] = new ResourceManager::FontResource(filename);
		} else {
			Logger::begin("Font", Logger::LogLevel_Error) << "Could not load font" << Logger::end();
            return false;
		}
        
        Draw2D::CheckGLError("PostLoadFont");
        
        Logger::begin("Font", Logger::LogLevel_Verbose) << "Loaded Font: " << filename << " as " << prettyName << Logger::end();
        
        return true;
    }
    
    bool Application::IsFontLoaded(std::string fontName) {
        return _fonts.count(fontName) != 0;
    }
    
    std::vector<std::string> Application::GetCommandLineArgs() {
        return _jsArgs;
    }
    
	bool Application::_runFile(std::string path, bool persist) {
		Logger::begin("Scripting", Logger::LogLevel_Verbose) << "Loading File: " << path << Logger::end();
        
		v8::HandleScope scp(v8::Isolate::GetCurrent());
        v8::Local<v8::Context> ctx = v8::Isolate::GetCurrent()->GetCurrentContext();
        v8::Context::Scope ctx_scope(ctx);
        
		char* inputScript = Filesystem::GetFileContent(path);
        
		v8::TryCatch tryCatch;
        
		v8::Handle<v8::Script> script = v8::Script::Compile(
                                                            v8::String::New(inputScript), v8::String::New(path.c_str()));
        
		if (script.IsEmpty()) {
			Logger::begin("Scripting", Logger::LogLevel_Error) << "Could not Load file: " << path << Logger::end();
            ScriptingManager::ReportException(v8::Isolate::GetCurrent(), &tryCatch);
            std::free(inputScript);
            return false;
		} else {
			script->Run();
            if (!tryCatch.StackTrace().IsEmpty()) {
                ScriptingManager::ReportException(v8::Isolate::GetCurrent(), &tryCatch);
                std::free(inputScript);
                return false;
            }
            Logger::begin("Scripting", Logger::LogLevel_Verbose) << "Loaded File: " << path << Logger::end();
            if (persist) {
                _loadedFiles[path] = Filesystem::GetFileModifyTime(path);
            }
            std::free(inputScript);
            return true;
		}
	}
    
    bool Application::RunFile(std::string path, bool persist) {
        if (!Filesystem::FileExists(path)) {
            Logger::begin("Scripting", Logger::LogLevel_Error) << path << " Not Found" << Logger::end();
            return false;
        } else {
            if (!_runFile(path, persist)) {
                Logger::begin("Scripting", Logger::LogLevel_Error) << "Could not load File : " << path << Logger::end();
                return false;
            } else {
                return true;
            }
        }
    }
    
    void Application::Exit() {
        this->_running = false;
    }
    
    void Application::RestartRenderer() {
        Logger::begin("Window", Logger::LogLevel_Log) << "Restarting renderer on next frame" << Logger::end();
        _restartNextFrame = true;
    }
    
    void Application::_restartRenderer() {
        Logger::begin("Window", Logger::LogLevel_Log) << "Restarting renderer" << Logger::end();
        this->_window->Reset();
        this->_initGLContext(this->_window->GetGraphicsVersion());
        this->UpdateScreen();
    }
    
    void Application::ToggleFullscreen() {
        Logger::begin("Window", Logger::LogLevel_Log) << "Switching to fullscreen on next frame" << Logger::end();
        this->_toggleNextframe = true;
    }
    
    void Application::_toggleFullscreen() {
        this->_window->SetFullscreen(!this->_window->GetFullscreen());
        this->UpdateScreen();
    }
    
	void Application::SaveScreenshot(std::string screenshotFilename) {
        this->_screenshotFilename = screenshotFilename;
        this->_screenshotNextframe = true;
    }
    
    void Application::_saveScreenshot() {
        int width, height;
        width = this->_window->GetWindowSize().x;
        height = this->_window->GetWindowSize().y;
        BYTE* pixels = new BYTE[4 * width * height];
        glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
        
        ImageWriter::SaveBufferToFile(this->_screenshotFilename, pixels, width, height);
        
        Logger::begin("Screenshot", Logger::LogLevel_Log)
        << "Saved Screenshot as: " << Filesystem::GetRealPath(_screenshotFilename)
        << Logger::end();
        
        Json::Value args(Json::objectValue);
        
        args["filename"] = Filesystem::GetRealPath(_screenshotFilename);
        
        Events::Emit("onSaveScreenshot", args);
        
        delete [] pixels;
    }
    
    bool Application::UsingGL3() {
        return this->_window->GetGlVersion().major > 3;
    }
    
    void Application::DumpProfile() {
        _dumpProfileAtFrameEnd = true;
    }
    
    void Application::_dumpProfile() {
        Profiler::DumpProfile();
    }
    
    void Application::RunCommand(std::string str) {
		v8::HandleScope scp(v8::Isolate::GetCurrent());
        v8::Local<v8::Context> ctx = v8::Isolate::GetCurrent()->GetCurrentContext();
        v8::Context::Scope ctx_scope(ctx);
        
		Logger::begin("Console", Logger::LogLevel_ConsoleInput) << "> " << str << Logger::end();
        
		v8::TryCatch tryCatch;
        
		v8::Handle<v8::Script> script = v8::Script::Compile(
                                                            v8::String::New(str.c_str()),
                                                            v8::String::NewSymbol("Console"));
        
		if (script.IsEmpty()) {
			v8::Handle<v8::Value> exception = tryCatch.StackTrace();
			v8::String::AsciiValue exception_str(exception);
            Logger::begin("Console", Logger::LogLevel_Error) << "Exception: " << *exception_str << Logger::end();
		} else {
            v8::Local<v8::Value> result = script->Run();
            if (*result != NULL && !result->IsExternal()) { // well it works
                if (result->IsUndefined() && !Config::GetBoolean("core.log.src.undefinedValue")) {
                    
                } else {
                    Logger::begin("Console", Logger::LogLevel_Log) << (result->IsNull() ? "null" : *v8::String::Utf8Value(result->ToString())) << Logger::end();
                }
            }
            if (!tryCatch.StackTrace().IsEmpty()) {
                // Use the old version, the new one is way too long
                v8::Handle<v8::Value> exception = tryCatch.StackTrace();
                v8::String::AsciiValue exception_str(exception);
                Logger::begin("Console", Logger::LogLevel_Error) << "Exception: " << *exception_str << Logger::end();
            }
		}
    }
    
    void Application::DetailProfile(int frames, std::string filename) {
        Profiler::ResetDetail();
        _detailFrames = frames;
        _detailFilename = filename;
    }
    
    void Application::InvalidateScript(std::string scriptName) {
        Logger::begin("Scripting", Logger::LogLevel_Verbose) << "Invalidating Script: " << scriptName << Logger::end();
        _loadedFiles[scriptName] = -1;
    }
    
    bool Application::GetKeyPressed(int key) {
        return this->_window->GetKeyStatus(key) == Key_Press;
    }
    
    OpenGLVersion Application::GetOpenGLVersion() {
        return this->_window->GetGlVersion();
    }
    
    EffectShaderTypes::Type Application::GetBestEffectShaderType() {
        return EffectShaderTypes::GLSL_150;
    }
    
    std::string Application::GetEngineVersion() {
        return std::string("Engine2D v ") + std::string(ENGINE_VERSION);
    }
    
    bool Application::IsDebugMode() {
        return this->_debugMode;
    }
    
    void Application::Assert(bool value, std::string reason, std::string line, int lineNumber) {
        if (value || !this->_debugMode) return;
        Logger::begin("Assert", Logger::LogLevel_Error) << "ASSERT FAILED : (" << line << ":" << lineNumber << ") " << reason << Logger::end();
        Platform::DumpStackTrace();
        exit(EXIT_FAILURE); // Can't recover from this
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
                    if (Config::GetBoolean("core.throttleOnIdle")) {
                        Platform::NanoSleep(150000);
                    }
                }
            }
            
            Profiler::StartProfileFrame();
            FramePerfMonitor::BeginFrame();
            Timer::Update(); // Timer events may be emited now, this is the soonest into the frame that Javascript can run
            Events::PollDeferedMessages(); // Events from other threads will run here by default, Javascript may run at this time
            this->_processScripts();
            
            if (this->_debugMode) {
                pthread_mutex_lock(&debugMesssageReadyMutex);
                if (debugMessageReady) {
                    this->_handleDebugMessage();
                    debugMessageReady = false;
                }
                pthread_mutex_unlock(&debugMesssageReadyMutex);
            }
            
            this->_checkUpdate();
            
            Profiler::Begin("Frame", Config::GetFloat("core.render.targetFrameTime"));
            
            this->_updateFrameTime();
            
            this->_updateMousePos();
            
            Profiler::Begin("Draw");
            
            Draw2D::CheckGLError("startOfRendering");
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            
            Draw2D::Begin2d();
            
            Profiler::Begin("EventDraw", Config::GetFloat("core.render.targetFrameTime") / 3 * 2);
            Events::Emit("draw"); // this is when most Javascript runs
            Profiler::End("EventDraw");
            
            Draw2D::End2d();
            
            Draw2D::Begin2d();
            
            Profiler::Begin("EngineUI", Config::GetFloat("core.render.targetFrameTime") / 3);
            EngineUI::Draw();
            Profiler::End("EngineUI");
            
            Draw2D::End2d();
            
            Profiler::End("Draw");
            
            Profiler::Begin("SwapBuffers");
            this->_window->Present();
            Profiler::End("SwapBuffers");
            
            Profiler::End("Frame");
            
			if (this->_window->ShouldClose()) {
				Logger::begin("Window", Logger::LogLevel_Log) << "Exiting" << Logger::end();
				this->_running = false;
				break;
			}
            
            Draw2D::CheckGLError("endOfRendering");
            
            if (Config::GetBoolean("core.script.gcOnFrame")) {
                Profiler::Begin("ScriptGC");
                v8::V8::IdleNotification();
                Profiler::End("ScriptGC");
            }
            
            if (this->_toggleNextframe) {
                Profiler::Begin("ToggleFullscreen");
                this->_toggleFullscreen();
                Profiler::End("ToggleFullscreen");
                this->_toggleNextframe = false;
            }
            
            if (this->_restartNextFrame) {
                Profiler::Begin("RestartRenderer");
                this->_restartRenderer();
                Profiler::End("RestartRenderer");
                this->_restartNextFrame = false;
            }
            
            if (this->_screenshotNextframe) {
                Profiler::Begin("SaveScreenshot");
                this->_saveScreenshot();
                Profiler::End("SaveScreenshot");
                this->_screenshotNextframe = false;
            }
            
            if (this->_dumpProfileAtFrameEnd) {
                this->_dumpProfile();
                this->_dumpProfileAtFrameEnd = false;
            }
            
            if (this->_detailFrames > 0) {
                Profiler::CaptureDetail();
                this->_detailFrames--;
                if (this->_detailFrames <= 0) {
                    std::string detailData = Profiler::GetDetailProfile();
                    Filesystem::WriteFile(_detailFilename, (char*) detailData.c_str(), sizeof(char) * detailData.length());
                    Logger::begin("Profiler", Logger::LogLevel_Log)
                    << "Saved Profiler Report as: " << Filesystem::GetRealPath(_detailFilename) << Logger::end();
                }
            }
            
            FramePerfMonitor::EndFrame();
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
        
        Events::Init();
        Config::EnableConfigEvents();
        this->_hookEvents();
        // The events system is now ready
        
        for (auto iter = this->_archivePaths.begin(); iter != this->_archivePaths.end(); iter++) {
            Filesystem::Mount(*iter);
        }
        
        // The filesystem is now ready for reading files
        
        this->_loadConfigFile();
        
        this->_applyDelayedConfigs();
        
        // Configuration is now setup
        
        if (this->_configVarsMode) {
            this->_printConfigVars();
            Filesystem::Destroy();
            return 0;
        }
        
        v8::HandleScope handle_scope(v8::Isolate::GetCurrent());
        
        Profiler::Begin("InitScripting");
        v8::Handle<v8::Context> ctx = this->_initScripting();
        Profiler::End("InitScripting");
        
        ctx->Enter();
        
        // Scripting has now initalized, Javascript may punch in during any event
        
        if (Config::GetBoolean("core.debug.v8Debug")) {
            _enableV8Debugger();
        }
        
        Profiler::Begin("InitOpenGL");
        this->_initOpenGL();
        Profiler::End("InitOpenGL");
        
        // The window is now ready
        
        Draw2D::CheckGLError("Post InitOpenGL");
        
        engine::EnableGLContext();
        
        this->_disablePreload();
        
        Events::Emit("postLoad");
        
        Draw2D::CheckGLError("On JS Post Load");
        
        FreeImage_Initialise();
        
        this->UpdateScreen();
        
        Draw2D::CheckGLError("Post Finish Loading");
        
        Logger::begin("Application", Logger::LogLevel_Highlight) << "Loaded" << Logger::end();
        
        this->_hookConfigs(); // this is the last stage of the boot process so previous code does'nt interfere.
        
        if (this->_testMode) {
            this->_loadTests();
            TestSuite::Run();
        } else {
            // Let's get this going
            this->_mainLoop();
        }
        
        engine::DisableGLContext();
        
		this->_shutdownOpenGL();
		this->_shutdownScripting();
        
		Filesystem::Destroy();
        
        JSMod::CloseAllOpenModules();
        
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
    
    Application* _singilton = NULL;
    
    Application* GetAppSingilton() {
        if (_singilton == NULL) {
            _singilton = new Application();
        }
        return _singilton;
    }
	
} // namespace Engine
