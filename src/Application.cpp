#include "Application.hpp"

#include <cstring>

#include "extern/OpenSans-Regular.ttf.hpp"

#include "extern/json/json.h"

#include "ResourceManager.hpp"

#include "TestSuite.hpp"

#include "FramePerfMonitor.hpp"

#include "PlatformTests.hpp"

#include "EngineUI.hpp"

#include "JSSys.hpp"
#include "JSDraw.hpp"
#include "JSInput.hpp"
#include "JSFS.hpp"
#include "JSDatabase.hpp"
#include "JSMod.hpp"
#include "JSUnsafe.hpp"

#include <pthread.h>

#include <v8-debug.h>

namespace Engine {
    
    static pthread_mutex_t debugMesssageReadyMutex;
    static bool debugMessageReady = false;
    
    // from https://github.com/glfw/glfw/blob/master/tests/events.c
    static std::string GLFW_GetKeyName(int key)
    {
        if (key < 256) {
			char* string = new char[2];
			string[0] = (char) key;
			string[1] = '\0';
            return std::string(string);
        }
        switch (key)
        {
                // Function keys
            case GLFW_KEY_ESCAPE:       return "ESCAPE";
            case GLFW_KEY_F1:           return "F1";
            case GLFW_KEY_F2:           return "F2";
            case GLFW_KEY_F3:           return "F3";
            case GLFW_KEY_F4:           return "F4";
            case GLFW_KEY_F5:           return "F5";
            case GLFW_KEY_F6:           return "F6";
            case GLFW_KEY_F7:           return "F7";
            case GLFW_KEY_F8:           return "F8";
            case GLFW_KEY_F9:           return "F9";
            case GLFW_KEY_F10:          return "F10";
            case GLFW_KEY_F11:          return "F11";
            case GLFW_KEY_F12:          return "F12";
            case GLFW_KEY_F13:          return "F13";
            case GLFW_KEY_F14:          return "F14";
            case GLFW_KEY_F15:          return "F15";
            case GLFW_KEY_F16:          return "F16";
            case GLFW_KEY_F17:          return "F17";
            case GLFW_KEY_F18:          return "F18";
            case GLFW_KEY_F19:          return "F19";
            case GLFW_KEY_F20:          return "F20";
            case GLFW_KEY_F21:          return "F21";
            case GLFW_KEY_F22:          return "F22";
            case GLFW_KEY_F23:          return "F23";
            case GLFW_KEY_F24:          return "F24";
            case GLFW_KEY_F25:          return "F25";
            case GLFW_KEY_UP:           return "UP";
            case GLFW_KEY_DOWN:         return "DOWN";
            case GLFW_KEY_LEFT:         return "LEFT";
            case GLFW_KEY_RIGHT:        return "RIGHT";
            case GLFW_KEY_LEFT_SHIFT:   return "LEFT SHIFT";
            case GLFW_KEY_RIGHT_SHIFT:  return "RIGHT SHIFT";
            case GLFW_KEY_LEFT_CONTROL: return "LEFT CONTROL";
            case GLFW_KEY_RIGHT_CONTROL: return "RIGHT CONTROL";
            case GLFW_KEY_LEFT_ALT:     return "LEFT ALT";
            case GLFW_KEY_RIGHT_ALT:    return "RIGHT ALT";
            case GLFW_KEY_TAB:          return "TAB";
            case GLFW_KEY_ENTER:        return "ENTER";
            case GLFW_KEY_BACKSPACE:    return "BACKSPACE";
            case GLFW_KEY_INSERT:       return "INSERT";
            case GLFW_KEY_DELETE:       return "DELETE";
            case GLFW_KEY_PAGE_UP:      return "PAGE UP";
            case GLFW_KEY_PAGE_DOWN:    return "PAGE DOWN";
            case GLFW_KEY_HOME:         return "HOME";
            case GLFW_KEY_END:          return "END";
            case GLFW_KEY_KP_0:         return "KEYPAD 0";
            case GLFW_KEY_KP_1:         return "KEYPAD 1";
            case GLFW_KEY_KP_2:         return "KEYPAD 2";
            case GLFW_KEY_KP_3:         return "KEYPAD 3";
            case GLFW_KEY_KP_4:         return "KEYPAD 4";
            case GLFW_KEY_KP_5:         return "KEYPAD 5";
            case GLFW_KEY_KP_6:         return "KEYPAD 6";
            case GLFW_KEY_KP_7:         return "KEYPAD 7";
            case GLFW_KEY_KP_8:         return "KEYPAD 8";
            case GLFW_KEY_KP_9:         return "KEYPAD 9";
            case GLFW_KEY_KP_DIVIDE:    return "KEYPAD DIVIDE";
            case GLFW_KEY_KP_MULTIPLY:  return "KEYPAD MULTPLY";
            case GLFW_KEY_KP_SUBTRACT:  return "KEYPAD SUBTRACT";
            case GLFW_KEY_KP_ADD:       return "KEYPAD ADD";
            case GLFW_KEY_KP_DECIMAL:   return "KEYPAD DECIMAL";
            case GLFW_KEY_KP_EQUAL:     return "KEYPAD EQUAL";
            case GLFW_KEY_KP_ENTER:     return "KEYPAD ENTER";
            case GLFW_KEY_PRINT_SCREEN: return "PRINT SCREEN";
            case GLFW_KEY_NUM_LOCK:     return "NUM LOCK";
            case GLFW_KEY_CAPS_LOCK:    return "CAPS LOCK";
            case GLFW_KEY_SCROLL_LOCK:  return "SCROLL LOCK";
            case GLFW_KEY_PAUSE:        return "PAUSE";
            case GLFW_KEY_LEFT_SUPER:   return "LEFT SUPER";
            case GLFW_KEY_RIGHT_SUPER:  return "RIGHT SUPER";
            case GLFW_KEY_MENU:         return "MENU";
                
            default:                    return NULL;
        }
    }
    
    static std::string GLFW_GetStateName(int state) {
        switch (state) {
            case GLFW_PRESS:    return "press";
            case GLFW_REPEAT:   return "repeat";
            case GLFW_RELEASE:  return "release";
            default:            return "unknown";
        }
    }
    
    // It's just the v8 code fitted closer to the engine's coding style
    static void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch) {
        std::stringstream ss;
        v8::HandleScope handle_scope(isolate);
        v8::String::Utf8Value exception(try_catch->Exception());
        const char* exception_string = *exception;
        v8::Handle<v8::Message> message = try_catch->Message();
        if (message.IsEmpty()) {
            // V8 didn't provide any extra information about this error; just
            // print the exception.
            ss << exception_string << std::endl;
        } else {
            // Print (filename):(line number): (message).
            v8::String::Utf8Value filename(message->GetScriptResourceName());
            const char* filename_string = *filename;
            int linenum = message->GetLineNumber();
            ss << filename_string << ":" << linenum << ": " << exception_string << std::endl;
            // Print line of source code.
            v8::String::Utf8Value sourceline(message->GetSourceLine());
            const char* sourceline_string = *sourceline;
            ss << sourceline_string << std::endl;
            // Print wavy underline (GetUnderline is deprecated).
            int start = message->GetStartColumn();
            for (int i = 0; i < start; i++) {
                ss << " ";
            }
            int end = message->GetEndColumn();
            for (int i = start; i < end; i++) {
                ss << "^";
            }
            v8::String::Utf8Value stack_trace(try_catch->StackTrace());
            if (stack_trace.length() > 0) {
                const char* stack_trace_string = *stack_trace;
                ss << std::endl << stack_trace_string;
            }
        }
        Logger::LogText("Scripting", Logger::LogLevel_Error, ss.str());
    }
    
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
	
#define addItem(table, js_name, funct) table->Set(js_name, v8::FunctionTemplate::New(funct))
    
    v8::Handle<v8::Context> Application::_initScripting() {
        Logger::begin("Scripting", Logger::LogLevel_Log) << "Loading Scripting" << Logger::end();
        
		v8::HandleScope handle_scope(v8::Isolate::GetCurrent());
        
		v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
        
        global->SetAccessor(v8::String::NewSymbol("global"), globalAccessor);
        
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
        
        v8::Handle<v8::Context> ctx = v8::Context::New(v8::Isolate::GetCurrent(), NULL, global);
        
        ctx->Enter();
        
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
        int debugPort = Config::GetInt("core.debug.v8DebugPort");
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
	
	void Application::_updateMousePos() {
		double x = 0;
		double y = 0;
		glfwGetCursorPos(window, &x, &y);
        
		v8::HandleScope scp(v8::Isolate::GetCurrent());
        v8::Local<v8::Context> ctx = v8::Isolate::GetCurrent()->GetCurrentContext();
        v8::Context::Scope ctx_scope(ctx);
        
		v8::Local<v8::Object> obj = v8::Context::GetCurrent()->Global();
		v8::Local<v8::Object> input_table = v8::Object::Cast(*obj->Get(v8::String::New("input")));
		input_table->Set(v8::String::New("mouseX"), v8::Number::New(x));
		input_table->Set(v8::String::New("mouseY"), v8::Number::New(y));
		input_table->Set(v8::String::New("leftMouseButton"), v8::Boolean::New(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS));
		input_table->Set(v8::String::New("rightMouseButton"), v8::Boolean::New(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS));
	}
	
	void Application::_updateScreen() {
		v8::HandleScope scp(v8::Isolate::GetCurrent());
        v8::Local<v8::Context> ctx = v8::Isolate::GetCurrent()->GetCurrentContext();
        v8::Context::Scope ctx_scope(ctx);
        
		v8::Local<v8::Object> obj = v8::Context::GetCurrent()->Global();
		v8::Local<v8::Object> sys_table = v8::Object::Cast(*obj->Get(v8::String::New("sys")));
		sys_table->Set(v8::String::New("screenWidth"), v8::Number::New(_screenWidth));
		sys_table->Set(v8::String::New("screenHeight"), v8::Number::New(_screenHeight));
	}
    
    void Application::_updateFrameTime() {
		v8::HandleScope scp(v8::Isolate::GetCurrent());
        v8::Local<v8::Context> ctx = v8::Isolate::GetCurrent()->GetCurrentContext();
        v8::Context::Scope ctx_scope(ctx);
        
		v8::Local<v8::Object> obj = v8::Context::GetCurrent()->Global();
		v8::Local<v8::Object> sys_table = v8::Object::Cast(*obj->Get(v8::String::New("sys")));
        sys_table->Set(v8::String::NewSymbol("deltaTime"), v8::Number::New(FramePerfMonitor::GetFrameTime()));
    }
    
    void Application::_disablePreload() {
		v8::HandleScope scp(v8::Isolate::GetCurrent());
        v8::Local<v8::Context> ctx = v8::Isolate::GetCurrent()->GetCurrentContext();
        v8::Context::Scope ctx_scope(ctx);
        
		v8::Local<v8::Object> obj = v8::Context::GetCurrent()->Global();
		v8::Local<v8::Object> sys_table = v8::Object::Cast(*obj->Get(v8::String::New("sys")));
        sys_table->Set(v8::String::New("preload"), v8::Boolean::New(false));
    }
    
    void Application::_loadBasicConfigs() {
        // new names in comments
        // I still need to think though and make sure these match up
        
        Config::SetBoolean( "core.runOnIdle",                       false);
        Config::SetBoolean( "core.catchErrors",                     !this->_debugMode);
        
        Config::SetNumber(  "core.window.width",                    800);
        Config::SetNumber(  "core.window.height",                   600);
        Config::SetBoolean( "core.render.aa",                       true);
        Config::SetBoolean( "core.window.vsync",                    false); // lack of vsync causes FPS issues
        Config::SetBoolean( "core.window.fullscreen",               false);
        Config::SetString(  "core.render.openGL",                   "3.2");
        Config::SetString(  "core.content.fontPath",                "fonts/OpenSans-Regular.ttf");
        Config::SetBoolean( "core.debug.engineUI.showVerboseLog",   false);
        Config::SetBoolean( "core.debug.engineUI",                  this->_developerMode);
        Config::SetBoolean( "core.debug.profiler",                  this->_developerMode || this->_debugMode);
        Config::SetString(  "core.window.title",                    "Engine2D");
        Config::SetBoolean( "core.debug.debugRenderer",             false);
        Config::SetString(  "core.render.basicShader",              "shaders/basic");
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
        
        Config::SetNumber(  "core.debug.v8DebugPort",               5858);
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
	
    void Application::_resizeWindow(GLFWwindow* window, int w, int h) {
        Application* app = GetAppSingilton();
        Logger::begin("Window", Logger::LogLevel_Verbose) << "Resizing Window to " << w << "x" << h << Logger::end();
		app->_screenWidth = w;
		app->_screenHeight = h;
		if (app->_running)
		{
			app->_updateScreen();
		}
		glViewport(0, 0, w, h);
        Draw2D::CheckGLError("Post Viewpoint");
    }
	
	 void Application::_keyPress(GLFWwindow* window, int rawKey, int scanCode, int state, int mods) {
        if (!glfwGetWindowAttrib(window, GLFW_FOCUSED) && !GetAppSingilton()->_isFullscreen) {
            return;
        }
        
        std::string key = GLFW_GetKeyName(rawKey);
        
        EngineUI::OnKeyPress(rawKey, state, mods & GLFW_MOD_SHIFT);
        
        // everything after this is to handle the JS Function.
        
        if (EngineUI::ConsoleActive()) {
            return;
        }
        
        Events::EventArgs e = Events::EventArgs({
            {"key", key},
            {"state", GLFW_GetStateName(state)},
            {"mod", (mods & GLFW_MOD_SHIFT) ? "true" : "false"}
        });
        
        Events::Emit("input", [&](Events::EventArgs e) {
            return e.equalsValue("key", key.c_str(), false);
        }, e);
	}
    
    void Application::_openWindow(int width, int height, bool fullscreen, std::string openGL3Context) {
        Logger::begin("Window", Logger::LogLevel_Log) << "Loading OpenGL : Init Window/Context" << Logger::end();
        
        if (openGL3Context == "3.2") {
            Logger::begin("Window", Logger::LogLevel_Warning) << "Loading OpenGL 3.2" << Logger::end();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            
            isGL3Context = true;
        } else {
            isGL3Context = false;
        }
        
        if (Config::GetBoolean("core.render.aa")) {
            glfwWindowHint(GLFW_SAMPLES, 4);
        }
        
        if (Config::GetBoolean("core.debug.debugRenderer")) {
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        }
        
		window = glfwCreateWindow(width, height, Config::GetString("core.window.title").c_str(), fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL); // you can resize how ever much you like
        
        if (window == NULL) {
            Logger::begin("Window", Logger::LogLevel_Error) << "Error Creating Window" << Logger::end();
        }
        
        glfwMakeContextCurrent(window);
        
        glfwSwapInterval(Config::GetBoolean("core.window.vsync") ? 1 : 0);
        
        Logger::begin("Window", Logger::LogLevel_Verbose) << "Loading OpenGL : Init GLEW" << Logger::end();
        
        Draw2D::CheckGLError("PostCreateContext");
        
        glewExperimental = GL_TRUE;
        
        GLenum err = glewInit();
        
        if (err != GLEW_OK) {
            Logger::begin("Window", Logger::LogLevel_Error) << "Error starting GLEW: " << glewGetErrorString(err) << Logger::end();
        }
        
        Draw2D::CheckGLError("Ignore Me, Mostly");
        
        Logger::begin("Window", Logger::LogLevel_Verbose) << "Loading OpenGL : Init Callbacks" << Logger::end();
        
        this->_resizeWindow(window, width, height);
        
        glfwSetWindowSizeCallback(window, this->_resizeWindow);
		glfwSetKeyCallback(window, this->_keyPress);
        
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        
        Logger::begin("Window", Logger::LogLevel_Verbose) << "Loading OpenGL : Init OpenGL State" << Logger::end();
        
        Draw2D::CheckGLError("PostSetCallback");
        
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
        
        if (!isGL3Context) {
            glDisable(GL_LIGHTING);
        }
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        if (Config::GetBoolean("core.debug.debugRenderer")) {
            glDebugMessageCallback(DebugMessageCallback, NULL);
        }
        
        Draw2D::CheckGLError("PostSetupContext");
        
        Logger::begin("Window", Logger::LogLevel_Log) << "Loaded OpenGL" << Logger::end();
        
        this->_initFonts();
        Draw2D::Init2d();
    }
    
    void Application::_closeWindow() {
        Logger::begin("Window", Logger::LogLevel_Log) << "Terminating Window" << Logger::end();
        this->_shutdownFonts();
        ResourceManager::UnloadAll();
        glfwDestroyWindow(window);
    }
	
	void Application::_initOpenGL() {
        Logger::begin("Window", Logger::LogLevel_Verbose) << "Loading OpenGL : Init GLFW" << Logger::end();
        
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
    }
    
	// semi-realtime time loading
	
	void Application::_checkUpdate() {
        if (Config::GetBoolean("core.script.autoReload")) {
            for(auto iterator = _loadedFiles.begin(); iterator != _loadedFiles.end(); iterator++) {
                long lastMod = Filesystem::GetFileModifyTime(iterator->first);
                if (lastMod > iterator->second) {
                    this->RunFile(iterator->first, true);
                }
            }
        } else {
            for(auto iterator = _loadedFiles.begin(); iterator != _loadedFiles.end(); iterator++) {
                if (iterator->second < 0) {
                    this->RunFile(iterator->first, true);
                }
            }
        }
	}
	
	// public methods
    
    bool Application::IsDelayedConfig(std::string configKey) {
        return this->_delayedConfigs.count(configKey) != 0;
    }
	
	int Application::GetScreenWidth() {
		return _screenWidth;
	}
	
	int Application::GetScreenHeight() {
		return _screenHeight;
	}

    void Application::SetScreenSize(int width, int height) {
        glfwSetWindowSize(window, width, height);
    }
	
	GLFT_Font* Application::GetFont(std::string fontName, int size) {
        return _fonts[fontName]->GetFont(size);
	}
    
    
    bool Application::LoadFont(std::string prettyName, std::string filename) {
        Logger::begin("Font", Logger::LogLevel_Log)
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
        
        Logger::begin("Font", Logger::LogLevel_Log) << "Loaded Font" << Logger::end();
        
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
            ReportException(v8::Isolate::GetCurrent(), &tryCatch);
            std::free(inputScript);
            return false;
		} else {
			script->Run();
            if (!tryCatch.StackTrace().IsEmpty()) {
                ReportException(v8::Isolate::GetCurrent(), &tryCatch);
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
        this->_closeWindow();
        this->_openWindow(Config::GetInt("core.window.width"), Config::GetInt("core.window.height"),
                          Config::GetBoolean("core.window.fullscreen"), Config::GetString("core.render.openGL"));
        this->_updateScreen();
    }
    
    void Application::ToggleFullscreen() {
        Logger::begin("Window", Logger::LogLevel_Log) << "Switching to fullscreen on next frame" << Logger::end();
        this->_toggleNextframe = true;
    }
    
    void Application::_toggleFullscreen() {
        if (this->_isFullscreen) {
            this->_closeWindow();
            this->_openWindow(Config::GetInt("core.window.width"), Config::GetInt("core.window.height"), false,
                              Config::GetString("core.render.openGL"));
            this->_isFullscreen = false;
            this->_updateScreen();
        } else {
            const GLFWvidmode* desktopMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            this->_closeWindow();
            this->_openWindow(desktopMode->width, desktopMode->height, true,
                              Config::GetString("core.render.openGL"));
            this->_isFullscreen = true;
            this->_updateScreen();
        }
    }
    
	void Application::SaveScreenshot(std::string screenshotFilename) {
        this->_screenshotFilename = screenshotFilename;
        this->_screenshotNextframe = true;
    }
    
    void Application::_saveScreenshot() {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        BYTE* pixels = new BYTE[3 * width * height];
        glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels);
        
        FIMEMORY* mem = FreeImage_OpenMemory();
        
        FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, width, height, ((((24 * width) + 23) / 24) * 3),
                                                       24, 0xFF0000, 0x00FF00, 0x0000FF, false);
        
        FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(_screenshotFilename.c_str());
        
        if (format == FIF_UNKNOWN) {
            format = FIF_BMP;
        }
        
        FreeImage_SaveToMemory(format, image, mem);
        
        long fileSize = FreeImage_TellMemory(mem);
        
        FreeImage_SeekMemory(mem, 0, SEEK_SET);
        
        char* buffer = (char*)malloc(sizeof(char) * fileSize);
        
        FreeImage_ReadMemory(buffer, sizeof(char), (unsigned int) fileSize, mem);
        
        Filesystem::WriteFile(_screenshotFilename, buffer, fileSize);
        
        Logger::begin("Screenshot", Logger::LogLevel_Log)
        << "Saved Screenshot as: " << Filesystem::GetRealPath(_screenshotFilename)
        << Logger::end();
        
        FreeImage_CloseMemory(mem);
        
        std::free(pixels);
    }
    
    bool Application::UsingGL3() {
        return isGL3Context;
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
            if (*result != NULL) { // well it works
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
        return glfwGetKey(window, key) == GLFW_PRESS;
    }
    
    OpenGLVersion Application::GetOpenGLVersion() {
        OpenGLVersion ret;
        
        ret.major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
        ret.minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
        ret.revision = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
        
        return ret;
    }
    
    void Application::_mainLoop() {
        this->_running = true;
        
		while (this->_running) {
            Profiler::StartProfileFrame();
            FramePerfMonitor::BeginFrame();
            
			if (!this->_isFullscreen &&
                !glfwGetWindowAttrib(window, GLFW_FOCUSED) &&
                !Config::GetBoolean("core.runOnIdle") &&
                !glfwWindowShouldClose(window)) {
				glfwWaitEvents();
                sleep(0);
				continue;
			}
            
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
            Events::Emit("draw", Events::EventArgs());
            Profiler::End("EventDraw");
            
            Profiler::Begin("EngineUI", Config::GetFloat("core.render.targetFrameTime") / 3);
            EngineUI::Draw();
            Profiler::End("EngineUI");
            
            Draw2D::End2d();
            
            Profiler::End("Draw");
            
            Profiler::Begin("SwapBuffers");
            glfwSwapBuffers(window);
            Profiler::End("SwapBuffers");
            
            Profiler::Begin("PollEvents");
            glfwPollEvents();
            Profiler::End("PollEvents");
            
            Profiler::End("Frame");
            
			if (glfwWindowShouldClose(window)) {
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
        Logger::begin("Application", Logger::LogLevel_Log) << "Starting" << Logger::end();
        
        if (this->_debugMode) {
            Logger::begin("Application", Logger::LogLevel_Warning) << "=== Debug Mode Active ===" << Logger::end();
        }
        
        if (this->_developerMode) {
            Logger::begin("Application", Logger::LogLevel_Warning) << "=== Developer Mode Active ===" << Logger::end();
        }
        
		Filesystem::Init(Platform::GetRawCommandLineArgV()[0]);
        
        for (auto iter = this->_archivePaths.begin(); iter != this->_archivePaths.end(); iter++) {
            Filesystem::Mount(*iter);
        }
        
        this->_loadConfigFile();
        
        this->_applyDelayedConfigs();
        
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
        
        if (this->_debugMode) {
            _enableV8Debugger();
        }
        
        Profiler::Begin("InitOpenGL");
        this->_initOpenGL();
        Profiler::End("InitOpenGL");
        
        Draw2D::CheckGLError("Post InitOpenGL");
        
        engine::EnableGLContext();
        
        this->_disablePreload();
        
        Events::Emit("postLoad", Events::EventArgs());
        
        Draw2D::CheckGLError("On JS Post Load");
        
        FreeImage_Initialise();
        
        this->_updateScreen();
        
        Draw2D::CheckGLError("Post Finish Loading");
        
        Logger::begin("Application", Logger::LogLevel_Log) << "Loaded" << Logger::end();
        
        if (this->_testMode) {
            this->_loadTests();
            TestSuite::Run();
        } else {
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
        Config::SetBoolean("core.log.enableConsole", true); // make sure we can log to the console right from the start
        
        Platform::SetRawCommandLine(argc, argv);
        
        if (!this->_parseCommandLine(argc, argv)) {
            return 1;
        }
        
        Config::SetBoolean("core.log.enableConsole", false);
        
        this->_loadBasicConfigs();
        
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
