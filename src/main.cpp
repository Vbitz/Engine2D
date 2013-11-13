#include "main.hpp"

#include <cstring>

#include "extern/OpenSans-Regular.ttf.hpp"

#include "ResourceManager.hpp"

#include "JSSys.hpp"
#include "JSDraw.hpp"
#include "JSInput.hpp"
#include "JSFS.hpp"
#include "JSDatabase.hpp"
#include "JSMod.hpp"

namespace Engine {
	
	// state
	bool running = false;
    
    bool isFullscreen = false;
    
    bool toggleNextframe = false;
    bool screenshotNextframe = false;
    bool _dumpProfileAtFrameEnd = false;
    bool _restartNextFrame = false;
    
    std::string _screenshotFilename;
	
	double lastTime = 0;
	
	int _screenWidth = 0;
	int _screenHeight = 0;
    
    std::vector<std::string> _jsArgs;
    
    bool isGL3Context;
    
    GLFWwindow* window = NULL;
	
    std::map<std::string, ResourceManager::FontResource*> _fonts;

    std::map<std::string, std::string> _delayedConfigs;
    
	// v8 Scripting
	v8::Isolate* _globalIsolate;

	v8::Persistent<v8::Context> _globalContext;
    
	std::map<std::string, long> _loadedFiles;
    
    bool _developerMode = false;
    
    int _detailFrames = 0;
    std::string _detailFilename = "";
    
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
    
    void InitFonts();
    void ShutdownFonts();
    
    ENGINE_JS_METHOD(SetWindowInitParams);
	
#define addItem(table, js_name, funct) table->Set(js_name, v8::FunctionTemplate::New(funct))
    
	void InitScripting(bool developerMode) {
        Logger::begin("Scripting", Logger::LogLevel_Log) << "Loading Scripting" << Logger::end();
        
		_globalIsolate = v8::Isolate::New();
		_globalIsolate->Enter();

		v8::HandleScope handle_scope;

		v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
	
        // consoleTable
        v8::Handle<v8::ObjectTemplate> consoleTable = v8::ObjectTemplate::New();
        
            addItem(consoleTable, "_log", JsSys::Println);
        
        global->Set("console", consoleTable);
        
		// sysTable
		v8::Handle<v8::ObjectTemplate> sysTable = v8::ObjectTemplate::New();
        
            JsSys::InitSys(sysTable);
        
            addItem(sysTable, "setWindowCreateParams", SetWindowInitParams);
        
            sysTable->Set("platform", v8::String::New(_PLATFORM));
            sysTable->Set("devMode", v8::Boolean::New(developerMode));
            sysTable->Set("preload", v8::Boolean::New(true));
            sysTable->Set("numProcessers", v8::Number::New(Platform::GetProcesserCount()));
	
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
        
		_globalContext = v8::Persistent<v8::Context>::New(_globalIsolate, v8::Context::New(_globalIsolate, NULL, global));
        
		if (!runFile(Config::GetString("script_bootloader"), true)) {
            Logger::begin("Scripting", Logger::LogLevel_Error) << "Bootloader not found" << Logger::end();
            EngineUI::ToggleConsole(); // give them something to debug using
        }
        
        Logger::begin("Scripting", Logger::LogLevel_Log) << "Loaded Scripting" << Logger::end();
	}
	
#undef addItem
	
	void ShutdownScripting() {
		_globalIsolate->Exit();
		_globalIsolate->Dispose();
	}
	
	bool CallFunction(v8::Handle<v8::Value> func) {
		v8::HandleScope scp;
		v8::Context::Scope ctx_scope(_globalContext);
	
		v8::Handle<v8::Function> real_func = v8::Handle<v8::Function>::Cast(func);
        
		v8::TryCatch tryCatch;
        
		real_func->Call(_globalContext->Global(), 0, NULL);
        
        if (!tryCatch.StackTrace().IsEmpty()) {
            v8::Handle<v8::Value> exception = tryCatch.StackTrace();
            v8::String::Utf8Value exception_str(exception);
            Logger::begin("Scripting", Logger::LogLevel_Error) << "Exception in C++ to JS Call: " << *exception_str << Logger::end();
            return false;
        }
        return true;
	}
	
	void UpdateMousePos() {
		double x = 0;
		double y = 0;
		glfwGetCursorPos(window, &x, &y);
	
		v8::HandleScope scp;
		v8::Context::Scope ctx_scope(_globalContext);
	
		v8::Local<v8::Object> obj = v8::Context::GetCurrent()->Global();
		v8::Local<v8::Object> input_table = v8::Object::Cast(*obj->Get(v8::String::New("input")));
		input_table->Set(v8::String::New("mouseX"), v8::Number::New(x));
		input_table->Set(v8::String::New("mouseY"), v8::Number::New(y));
		input_table->Set(v8::String::New("leftMouseButton"), v8::Boolean::New(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS));
		input_table->Set(v8::String::New("rightMouseButton"), v8::Boolean::New(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS));
	}
	
	void UpdateScreen() {
		v8::HandleScope scp;
		v8::Context::Scope ctx_scope(_globalContext);
	
		v8::Local<v8::Object> obj = v8::Context::GetCurrent()->Global();
		v8::Local<v8::Object> input_table = v8::Object::Cast(*obj->Get(v8::String::New("sys")));
		input_table->Set(v8::String::New("screenWidth"), v8::Number::New(_screenWidth));
		input_table->Set(v8::String::New("screenHeight"), v8::Number::New(_screenHeight));
	}
    
    void UpdateFrameTime() {
		v8::HandleScope scp;
		v8::Context::Scope ctx_scope(_globalContext);
        
		v8::Local<v8::Object> obj = v8::Context::GetCurrent()->Global();
		v8::Local<v8::Object> sys_table = v8::Object::Cast(*obj->Get(v8::String::New("sys")));
        sys_table->Set(v8::String::NewSymbol("deltaTime"), v8::Number::New(Profiler::GetTime("Frame")));
    }
    
    void DisablePreload() {
        v8::HandleScope scp;
        v8::Context::Scope ctx_scope(_globalContext);
        
		v8::Local<v8::Object> obj = v8::Context::GetCurrent()->Global();
		v8::Local<v8::Object> sys_table = v8::Object::Cast(*obj->Get(v8::String::New("sys")));
        sys_table->Set(v8::String::New("preload"), v8::Boolean::New(false));
    }

    void LoadBasicConfigs(bool developerMode) {
        Config::SetNumber(  "cl_width",                 800);
        Config::SetNumber(  "cl_height",                600);
        Config::SetBoolean( "cl_aa",                    true);
        Config::SetBoolean( "cl_vsync",                 false); // lack of vsync causes FPS issues
        Config::SetBoolean( "cl_fullscreen",            false);
        Config::SetBoolean( "cl_openGL3",               false);
        Config::SetString(  "cl_fontPath",              "fonts/OpenSans-Regular.ttf");
        Config::SetBoolean( "cl_showVerboseLog",        false);
        Config::SetBoolean( "cl_runOnIdle",             false);
        Config::SetBoolean( "cl_engineUI",              developerMode);
        Config::SetBoolean( "cl_profiler",              developerMode);
        Config::SetBoolean( "cl_scriptedDraw",          true);
        Config::SetString(  "cl_title",                 "Engine2D");
        Config::SetBoolean( "cl_debugContext",          developerMode);
        Config::SetString(  "cl_gl3Shader",             "shaders/basic");
        Config::SetNumber(  "cl_targetFrameTime",       1.0f / 30.0f);
        
        Config::SetBoolean( "draw_clampCreateTexture",  true);
        Config::SetBoolean( "draw_createImageMipmap",   true);
        
        Config::SetBoolean( "script_reload",            developerMode);
        Config::SetBoolean( "script_gcFrame",           developerMode);
        Config::SetString(  "script_bootloader",        "lib/boot.js");
        Config::SetString(  "script_config",            "config/config.json");
        
#ifdef PLATFORM_WINDOWS
        Config::SetBoolean( "log_console",              developerMode);
#else
        Config::SetBoolean( "log_console",              true);
#endif
        Config::SetBoolean( "log_file",                 "");
        Config::SetBoolean( "log_consoleVerbose",       developerMode);
        Config::SetBoolean( "log_colors",               true);
        Config::SetBoolean( "log_script_undefined",     developerMode);
        Config::SetBoolean( "log_profiler_maxTime",     developerMode);
        Config::SetBoolean( "log_createImage",          true);
    }
	
	void ResizeWindow(GLFWwindow* window, int w, int h) {
        Logger::begin("Window", Logger::LogLevel_Verbose) << "Resizing Window to " << w << "x" << h << Logger::end();
		_screenWidth = w;
		_screenHeight = h;
		if (running)
		{
			UpdateScreen();
		}
		glViewport(0, 0, w, h);
        Draw2D::CheckGLError("Post Viewpoint");
    }
	
	void KeyPress(GLFWwindow* window, int rawKey, int scanCode, int state, int mods) {
        if (!glfwGetWindowAttrib(window, GLFW_FOCUSED) && !isFullscreen) {
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
            {"state", GLFW_GetStateName(state)}
        });
        
        Events::Emit("input", [&](Events::EventArgs e) {
            return e.equalsValue("key", key.c_str(), false);
        }, e);
	}
    
    void OnGLFWError(int error, const char* msg) {
        Logger::begin("Window", Logger::LogLevel_Error) << "GLFW Error : " << error << " : " << msg << Logger::end();
    }
    
    void DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, void* userParam) {
        Logger::begin("OpenGL", Logger::LogLevel_Error) << source << " : " << type << " : " <<
            id << " : " << severity << " : " << message << Logger::end();
    }
    
    ENGINE_JS_METHOD(SetWindowInitParams) {
        ENGINE_JS_SCOPE_OPEN;
        
        ENGINE_CHECK_ARGS_LENGTH(1);
        
        ENGINE_CHECK_ARG_OBJECT(0, "Arg0 is the object to set the window creation values with");
        
        v8::Object* obj = ENGINE_GET_ARG_OBJECT(0);
        
        v8::Local<v8::Array> objNames = obj->GetPropertyNames();
        
        for (int i = 0; i < objNames->Length(); i++) {
            v8::Local<v8::String> objKey = objNames->Get(i)->ToString();
            v8::Local<v8::Value> objItem = obj->Get(objKey);
            std::string objKeyValue = std::string(*v8::String::Utf8Value(objKey));
            if (_delayedConfigs.count(objKeyValue) != 0) {
                continue; // ignore it
            }
            if (objItem->IsString()) {
                Config::SetString(objKeyValue,
                                  std::string(*v8::String::Utf8Value(objItem)));
            } else if (objItem->IsNumber()) {
                Config::SetNumber(objKeyValue,
                                  (float) objItem->NumberValue());
            } else if (objItem->IsBoolean()) {
                Config::SetBoolean(objKeyValue,
                                  (float) objItem->BooleanValue());
            } else {
                ENGINE_THROW_ARGERROR("Invalid value, values must be a number, string or boolean");
            }
        }
        
        ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
    }
    
    void OpenWindow(int width, int height, bool fullscreen, bool openGL3Context) {
        Logger::begin("Window", Logger::LogLevel_Log) << "Loading OpenGL : Init Window/Context" << Logger::end();
        
        if (openGL3Context) {
            Logger::begin("Window", Logger::LogLevel_Warning) << "Loading OpenGL 3.2" << Logger::end();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            
            isGL3Context = true;
        } else {
            isGL3Context = false;
        }
        
        if (Config::GetBoolean("cl_aa")) {
            glfwWindowHint(GLFW_SAMPLES, 4);
        }
        
        if (Config::GetBoolean("cl_debugContext")) {
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        }
        
		window = glfwCreateWindow(width, height, Config::GetString("cl_title").c_str(), fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL); // you can resize how ever much you like
        
        if (window == NULL) {
            Logger::begin("Window", Logger::LogLevel_Error) << "Error Creating Window" << Logger::end();
        }
        
        glfwMakeContextCurrent(window);
        
        glfwSwapInterval(Config::GetBoolean("cl_vsync") ? 1 : 0);
        
        Logger::begin("Window", Logger::LogLevel_Verbose) << "Loading OpenGL : Init GLEW" << Logger::end();
        
        Draw2D::CheckGLError("PostCreateContext");
        
        glewExperimental = GL_TRUE;
        
        GLenum err = glewInit();
        
        if (err != GLEW_OK) {
            Logger::begin("Window", Logger::LogLevel_Error) << "Error starting GLEW: " << glewGetErrorString(err) << Logger::end();
        }
        
        Draw2D::CheckGLError("Ignore Me, Mostly");
        
        Logger::begin("Window", Logger::LogLevel_Verbose) << "Loading OpenGL : Init Callbacks" << Logger::end();
        
        ResizeWindow(window, width, height);
        
        glfwSetWindowSizeCallback(window, ResizeWindow);
		glfwSetKeyCallback(window, KeyPress);
        
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        
        Logger::begin("Window", Logger::LogLevel_Verbose) << "Loading OpenGL : Init OpenGL State" << Logger::end();
        
        Draw2D::CheckGLError("PostSetCallback");
        
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
        
        if (!isGL3Context) {
            glDisable(GL_LIGHTING);
        }
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        if (Config::GetBoolean("cl_debugContext")) {
            glDebugMessageCallback(DebugMessageCallback, NULL);
        }
        
        Draw2D::CheckGLError("PostSetupContext");
        
        Logger::begin("Window", Logger::LogLevel_Log) << "Loaded OpenGL" << Logger::end();
        
        InitFonts();
        Draw2D::Init2d();
    }
    
    void CloseWindow() {
        Logger::begin("Window", Logger::LogLevel_Log) << "Terminating Window" << Logger::end();
        ShutdownFonts();
        ResourceManager::UnloadAll();
        glfwDestroyWindow(window);
    }
	
	void InitOpenGL() {
        Logger::begin("Window", Logger::LogLevel_Verbose) << "Loading OpenGL : Init GLFW" << Logger::end();
        
        glfwSetErrorCallback(OnGLFWError);
        
		glfwInit();
        
        OpenWindow(Config::GetInt("cl_width"), Config::GetInt("cl_height"),
                   Config::GetBoolean("cl_fullscreen"), Config::GetBoolean("cl_openGL3"));
	}
	
	void ShutdownOpenGL() {
        CloseWindow();
		glfwTerminate();
	}
	
	// font rendering
	
    bool loadFont(std::string prettyName, std::string filename) {
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
    
	void InitFonts() {
        Profiler::Begin("LoadFonts");
        if (!loadFont("basic", Config::GetString("cl_fontPath"))) {
            Logger::begin("Font", Logger::LogLevel_Warning) << "Font not found: " << Config::GetString("cl_fontPath") << " falling back to inbuilt font" << Logger::end();
            ResourceManager::Load("basicFont", new ResourceManager::RawSource(OpenSans_Regular, sizeof(OpenSans_Regular)));
            _fonts["basic"] = new ResourceManager::FontResource("basicFont");
        }
        Profiler::End("LoadFonts");
	}
	
	void ShutdownFonts() {
        _fonts.clear();
	}
    
    // command line handlers
    
    void ApplyDelayedConfigs() {
        for (auto iter = _delayedConfigs.begin(); iter != _delayedConfigs.end(); iter++) {
            if (!Config::Set(iter->first, iter->second)) {
                Logger::begin("CommandLine", Logger::LogLevel_Error) << "Could not set '"
                    << iter->first << "' to '" << iter->second
                    << "' Ignoring" << Logger::end();
            }
        }
    }
    
    // command line parsing
    
    bool ParseCommandLine(int argc, const char* argv[]) {
        /*
         Command Line Spec
         ========================
         
         -- Examples --
         bin/Engine - loads config/config.json and sets basic config values
         bin/Engine -Ccl_width=1280 -Ccl_height=800 - loads config/config.json and
            overrides the basic configs cl_width and cl_height
         bin/Engine -config=config/test.json - loads config/test.json and sets basic config values
         bin/Engine script/test - any non - configs are passed onto javascript
         
         -- Args --
               -Cname=value             - Overloads a basic config. This is applyed before loading the basic config
                                            but overrides those configs while they are applyed
         (NYI) -config=configFile       - Sets the basic config to configFile, configFile is realitive to res/
                                            since it uses PhysFS this could be other values
         (NYI) -archive=archiveFile     - Loads a archive file using PhysFS, this is applyed after physfs is started
         (NYI) -test                    - Runs the built in test suite
         (NYI) -headless                - Loads scripting without creating a OpenGL context, any calls requiring OpenGL
                                            will fail.
               -devmode                 - Enables developer mode (This enables real time script loading and the console)
         (NYI) -debug                   - Enables debug mode (This enables a OpenGL debug context and will print messages
                                            to the console)
         (NYI) -log=filename            - Logs logger output to filename (This is not writen using PhysFS so it needs a
                                            regular path)
         (NYI) -h                       - Prints this message
         */
        
        bool _exit = false;
        
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-devmode") == 0) {
                // enable devmode
                _developerMode = true;
            } else if (strcmp(argv[i], "-debug") == 0) {
                // enable debug
            } else if (strcmp(argv[i], "-test") == 0) {
                // start test mode
            } else if (strcmp(argv[i], "-headless") == 0) {
                // enable headless mode
            } else if (strcmp(argv[i], "-h") == 0) {
                // display help and exit
                _exit = true;
            } else if (strncmp(argv[i], "-C", 2) == 0) {
                // set delayed config
                size_t keyLength = strcspn(argv[i], "=") - 2;
                size_t valueLength = strlen(argv[i]) - keyLength - 3; // 2 chars for -C and 1 for =
                
                char* key = (char*) malloc(keyLength);
                strncpy(key, &argv[i][2], keyLength);
                
                char* value = (char*) malloc(valueLength);
                strncpy(value, &argv[i][2 + keyLength + 1], valueLength);
                
                _delayedConfigs[key] = std::string(value);
                
                free(key);
                free(value);
            } else if (strncmp(argv[i], "-config=", 8) == 0) {
                // set config filename
            } else if (strncmp(argv[i], "-archive=", 9) == 0) {
                // add archive to PhysFS after PhysFS Init
            } else if (strncmp(argv[i], "-log=", 2) == 0) {
                // set logfile and enable logging
            } else {
                // push to JS args
                _jsArgs.push_back(argv[i]);
            }
        }
        
        return !_exit;
    }
	
	// semi-realtime time loading
	
	void CheckUpdate() {
        for(auto iterator = _loadedFiles.begin(); iterator != _loadedFiles.end(); iterator++) {
			long lastMod = Filesystem::GetFileModifyTime(iterator->first);
			if (lastMod > iterator->second) {
                runFile(iterator->first, true);
			}
		}
	}
	
	// public methods
	
	int getScreenWidth() {
		return _screenWidth;
	}
	
	int getScreenHeight() {
		return _screenHeight;
	}
	
	GLFT_Font* getFont(std::string fontName, int size) {
        return _fonts[fontName]->GetFont(size);
	}
    
    bool isFontLoaded(std::string fontName) {
        return _fonts.count(fontName) != 0;
    }
    
    v8::Persistent<v8::Context> getGlobalContext() {
        return _globalContext;
    }
    
    std::vector<std::string> getCommandLineArgs() {
        return _jsArgs;
    }
    
	bool _runFile(std::string path, bool persist) {
        v8::HandleScope handle_scope;
        
		Logger::begin("Scripting", Logger::LogLevel_Log) << "Loading File: " << path << Logger::end();

		v8::Context::Scope ctx_scope(_globalContext);

		char* inputScript = Filesystem::GetFileContent(path);

		v8::TryCatch tryCatch;

		v8::Handle<v8::Script> script = v8::Script::Compile(
            v8::String::New(inputScript), v8::String::New(path.c_str()));

		if (script.IsEmpty()) {
			Logger::begin("Scripting", Logger::LogLevel_Error) << "Could not Load file: " << path << Logger::end();
			v8::Handle<v8::Value> exception = tryCatch.StackTrace();
			v8::String::AsciiValue exception_str(exception);
            Logger::begin("Scripting", Logger::LogLevel_Error) << "Exception: " << *exception_str << Logger::end();
            std::free(inputScript);
            return false;
		} else {
			script->Run();
            if (!tryCatch.StackTrace().IsEmpty()) {
                v8::Handle<v8::Value> exception = tryCatch.StackTrace();
                v8::String::AsciiValue exception_str(exception);
                Logger::begin("Scripting", Logger::LogLevel_Error) << "Exception: " << *exception_str << Logger::end();
                std::free(inputScript);
                return false;
            }
            Logger::begin("Scripting", Logger::LogLevel_Log) << "Loaded File: " << path << Logger::end();
            if (persist) {
                _loadedFiles[path] = Filesystem::GetFileModifyTime(path);
            }
            std::free(inputScript);
            return true;
		}
	}
    
    bool runFile(std::string path, bool persist) {
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
    
    void exit() {
        running = false;
    }
    
    void restartRenderer() {
        Logger::begin("Window", Logger::LogLevel_Log) << "Restarting renderer on next frame" << Logger::end();
        _restartNextFrame = true;
    }
    
    void _restartRenderer() {
        CloseWindow();
        OpenWindow(Config::GetInt("cl_width"), Config::GetInt("cl_height"),
                   Config::GetBoolean("cl_fullscreen"), Config::GetBoolean("cl_openGL3"));
        UpdateScreen();
    }
    
    void toggleFullscreen() {
        Logger::begin("Window", Logger::LogLevel_Log) << "Switching to fullscreen on next frame" << Logger::end();
        toggleNextframe = true;
    }
    
    void _toggleFullscreen() {
        if (isFullscreen) {
            CloseWindow();
            OpenWindow(Config::GetInt("cl_width"), Config::GetInt("cl_height"), false, isGL3Context);
            isFullscreen = false;
            UpdateScreen();
        } else {
            const GLFWvidmode* desktopMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            CloseWindow();
            OpenWindow(desktopMode->width, desktopMode->height, true, isGL3Context);
            isFullscreen = true;
            UpdateScreen();
        }
    }

	void saveScreenshot(std::string screenshotFilename) {
        _screenshotFilename = screenshotFilename;
        screenshotNextframe = true;
    }
    
    void _saveScreenshot() {
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
    
    void upgradeGL3() {
        int width, height;
        
        glfwGetWindowSize(window, &width, &height);
        
        CloseWindow();
        OpenWindow(width, height, isFullscreen, true);
    }
    
    bool usingGL3() {
        return isGL3Context;
    }
    
    GLFWwindow* getGLFWWindow() {
        return window;
    }
    
    void dumpProfile() {
        _dumpProfileAtFrameEnd = true;
    }
    
    void _dumpProfile() {
        Profiler::DumpProfile();
    }
    
    void runCommand(std::string str) {
        v8::HandleScope handle_scope;
        
		Logger::begin("Console", Logger::LogLevel_ConsoleInput) << "> " << str << Logger::end();
        
		v8::Context::Scope ctx_scope(_globalContext);
        
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
                if (result->IsUndefined() && !Config::GetBoolean("log_script_undefined")) {
                    
                } else {
                    Logger::begin("Console", Logger::LogLevel_Log) << (result->IsNull() ? "null" : *v8::String::Utf8Value(result->ToString())) << Logger::end();
                }
            }
            if (!tryCatch.StackTrace().IsEmpty()) {
                v8::Handle<v8::Value> exception = tryCatch.StackTrace();
                v8::String::AsciiValue exception_str(exception);
                Logger::begin("Console", Logger::LogLevel_Error) << "Exception: " << *exception_str << Logger::end();
            }
		}
    }
    
    void detailProfile(int frames, std::string filename) {
        Profiler::ResetDetail();
        _detailFrames = frames;
        _detailFilename = filename;
    }
    
    void invalidateScript(std::string scriptName) {
        _loadedFiles[scriptName] = 0;
    }
    
    void MainLoop() {
        running = true;
        
		while (running) {
            Profiler::StartProfileFrame();
            
			if (!isFullscreen &&
                    !glfwGetWindowAttrib(window, GLFW_FOCUSED) &&
                    !Config::GetBoolean("cl_runOnIdle") &&
                    !glfwWindowShouldClose(window)) {
				glfwWaitEvents();
                sleep(0);
				continue;
			}
            
            if (Config::GetBoolean("script_reload")) {
                CheckUpdate();
            }
            
            Profiler::Begin("Frame", Config::GetFloat("cl_targetFrameTime"));
            
            UpdateFrameTime();
            
            UpdateMousePos();
            
            Profiler::Begin("Draw");
            
            Draw2D::CheckGLError("startOfRendering");
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            
            Draw2D::Begin2d();
            
            if (Config::GetBoolean("cl_scriptedDraw")) {
                Profiler::Begin("JSDraw", Config::GetFloat("cl_targetFrameTime") / 3 * 2);
                Events::Emit("draw", Events::EventArgs());
                Profiler::End("JSDraw");
            }
            
            Profiler::Begin("EngineUI", Config::GetFloat("cl_targetFrameTime") / 3);
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
				running = false;
				break;
			}
            
            Draw2D::CheckGLError("endOfRendering");
            
            if (Config::GetBoolean("script_gcFrame")) {
                Profiler::Begin("ScriptGC");
                v8::V8::IdleNotification();
                Profiler::End("ScriptGC");
            }
            
            if (toggleNextframe) {
                Profiler::Begin("ToggleFullscreen");
                _toggleFullscreen();
                Profiler::End("ToggleFullscreen");
                toggleNextframe = false;
            }
            
            if (_restartNextFrame) {
                Profiler::Begin("RestartRenderer");
                _restartRenderer();
                Profiler::End("RestartRenderer");
                _restartNextFrame = false;
            }
            
            if (screenshotNextframe) {
                Profiler::Begin("SaveScreenshot");
                _saveScreenshot();
                Profiler::End("SaveScreenshot");
                screenshotNextframe = false;
            }
            
            if (_dumpProfileAtFrameEnd) {
                _dumpProfile();
                _dumpProfileAtFrameEnd = false;
            }
            
            if (_detailFrames > 0) {
                Profiler::CaptureDetail();
                _detailFrames--;
                if (_detailFrames <= 0) {
                    std::string detailData = Profiler::GetDetailProfile();
                    Filesystem::WriteFile(_detailFilename, (char*) detailData.c_str(), sizeof(char) * detailData.length());
                    Logger::begin("Profiler", Logger::LogLevel_Log)
                    << "Saved Profiler Report as: " << Filesystem::GetRealPath(_detailFilename) << Logger::end();
                }
            }
		}
    }
    
	// main function
	
	int main(int argc, char const *argv[]) {
        Platform::SetRawCommandLine(argc, argv);
        
        if (!ParseCommandLine(argc, argv)) {
            return 1;
        }
        
        LoadBasicConfigs(_developerMode);
        
        ApplyDelayedConfigs();
        
        Logger::begin("Application", Logger::LogLevel_Log) << "Starting" << Logger::end();
        
		Filesystem::Init(argv[0]);
        
        Profiler::Begin("InitScripting");
            InitScripting(_developerMode);
        Profiler::End("InitScripting");
        
        Profiler::Begin("InitOpenGL");
            InitOpenGL();
        Profiler::End("InitOpenGL");
        
        Draw2D::CheckGLError("Post InitOpenGL");
        
        engine::EnableGLContext();
        
        DisablePreload();
        
        Events::Emit("postLoad", Events::EventArgs());
        
        Draw2D::CheckGLError("On JS Post Load");
        
        FreeImage_Initialise();
        
        UpdateScreen();
        
        Draw2D::CheckGLError("Post Finish Loading");
        
        Logger::begin("Application", Logger::LogLevel_Log) << "Loaded" << Logger::end();
        
        MainLoop();
        
        engine::DisableGLContext();
        
		ShutdownOpenGL();
		ShutdownScripting();

		Filesystem::Destroy();
        
        JSMod::CloseAllOpenModules();

		return 0;
	}
	
} // namespace Engine

int main(int argc, char const *argv[])
{
	return Engine::main(argc, argv);
}
