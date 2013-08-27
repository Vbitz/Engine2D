#include "main.hpp"
	
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
    
    int _argc;
    const char** _argv;
    
    bool isGL3Context;
    
    GLFWwindow* window = NULL;
	
    std::map<std::string, ResourceManager::FontResource*> _fonts;
	
	std::map<std::string, std::string> _persists;

	// v8 Scripting
	v8::Isolate* _globalIsolate;

	v8::Persistent<v8::Context> _globalContext;
	
	v8::Persistent<v8::Function> _drawFunc;
	v8::Persistent<v8::Function> _keyboardFunc;
    v8::Persistent<v8::Function> _onPostLoadFunc;
    
	std::map<std::string, long> _loadedFiles;
    
    bool _developerMode = true;
    
    int _detailFrames = 0;
    std::string _detailFilename = "";
    
    // from https://github.com/glfw/glfw/blob/master/tests/events.c
    static std::string GLFW_GetKeyName(int key)
    {
        if (key < 256) {
            return std::string({ (char) key, '\0'});
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

		_globalContext = v8::Persistent<v8::Context>::New(_globalIsolate, v8::Context::New(_globalIsolate, NULL, global));
        
		runFile(Config::GetString("script_bootloader"), true);
        
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
            v8::String::AsciiValue exception_str(exception);
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

    void LoadBasicConfigs(bool developerMode) {
        Config::SetNumber(  "cl_width",                 800);
        Config::SetNumber(  "cl_height",                600);
        Config::SetBoolean( "cl_aa",                    true);
        Config::SetBoolean( "cl_vsync",                 true); // lack of vsync causes FPS issues
        Config::SetBoolean( "cl_fullscreen",            false);
        Config::SetBoolean( "cl_openGL3",               false);
        Config::SetString(  "cl_fontPath",              "fonts/OpenSans-Regular.ttf");
        Config::SetString(  "cl_consoleFontPath",       "fonts/SourceSansPro-Regular.ttf");
        Config::SetBoolean( "cl_showVerboseLog",        false);
        Config::SetBoolean( "cl_runOnIdle",             false);
        Config::SetBoolean( "cl_engineUI",              developerMode);
        Config::SetBoolean( "cl_profiler",              developerMode);
        Config::SetBoolean( "cl_scriptedDraw",          true);
        
        Config::SetBoolean( "draw_clampCreateTexture",  true);
        Config::SetBoolean( "draw_createImageMipmap",   true);
        
        Config::SetBoolean( "script_reload",            developerMode);
        Config::SetBoolean( "script_gcFrame",           true);
        Config::SetString(  "script_bootloader",        "lib/boot.js");
        
        Config::SetBoolean( "log_console",              true);
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
        
        if (_keyboardFunc.IsEmpty() || EngineUI::ConsoleActive()) {
            return;
        }
        
        v8::HandleScope scp;
        v8::Context::Scope ctx_scope(_globalContext);
        
        v8::TryCatch tryCatch;
        
        v8::Handle<v8::Function> real_func = v8::Handle<v8::Function>::Cast(_keyboardFunc);

        v8::Handle<v8::Value> argv[3];
        
        argv[0] = v8::String::NewSymbol(key.length() > 1 ? "special" : "char");
        argv[1] = v8::String::NewSymbol(key.c_str());
        argv[2] = v8::Boolean::New(state == GLFW_PRESS || state == GLFW_REPEAT);
        
        real_func->Call(_globalContext->Global(), 3, argv);
        
        if (!tryCatch.StackTrace().IsEmpty()) {
            v8::Handle<v8::Value> exception = tryCatch.StackTrace();
            v8::String::AsciiValue exception_str(exception);
            Logger::begin("Scripting", Logger::LogLevel_Error) << "Exception in C++ to JS Call: " << *exception_str << Logger::end();
            _keyboardFunc.Clear();
        }
	}
    
    void OnGLFWError(int error, const char* msg) {
        Logger::begin("Window", Logger::LogLevel_Error) << "GLFW Error : " << error << " : " << msg << Logger::end();
    }
    
    ENGINE_JS_METHOD(SetWindowInitParams) {
        ENGINE_JS_SCOPE_OPEN;
        
        ENGINE_CHECK_ARGS_LENGTH(1);
        
        ENGINE_CHECK_ARG_OBJECT(0, "Arg0 is the object to set the window creation values with");
        
        v8::Object* obj = ENGINE_GET_ARG_OBJECT(0);
        
        if (obj->Has(v8::String::NewSymbol("width")) && obj->Get(v8::String::NewSymbol("width"))->IsInt32()) {
            Config::SetNumber("cl_width", obj->Get(v8::String::NewSymbol("width"))->Int32Value());
        }
        
        if (obj->Has(v8::String::NewSymbol("height")) && obj->Get(v8::String::NewSymbol("height"))->IsInt32()) {
            Config::SetNumber("cl_height", obj->Get(v8::String::NewSymbol("height"))->Int32Value());
        }
        
        if (obj->Has(v8::String::NewSymbol("fullscreen")) && obj->Get(v8::String::NewSymbol("fullscreen"))->IsBoolean()) {
            Config::SetBoolean("cl_fullscreen", obj->Get(v8::String::NewSymbol("fullscreen"))->BooleanValue());
        }
        
        if (obj->Has(v8::String::NewSymbol("openGL3")) && obj->Get(v8::String::NewSymbol("openGL3"))->IsBoolean()) {
            Config::SetBoolean("cl_openGL3", obj->Get(v8::String::NewSymbol("openGL3"))->BooleanValue());
        }
        
        if (obj->Has(v8::String::NewSymbol("fontPath")) && obj->Get(v8::String::NewSymbol("fontPath"))->IsString()) {
            Config::SetString("cl_fontPath",
                        std::string(*v8::String::AsciiValue(obj->Get(v8::String::NewSymbol("fontPath"))->ToString())));
        }
        
        if (obj->Has(v8::String::NewSymbol("consoleFontPath")) && obj->Get(v8::String::NewSymbol("consoleFontPath"))->IsString()) {
            Config::SetString("cl_consoleFontPath",
                        std::string(*v8::String::AsciiValue(obj->Get(v8::String::NewSymbol("consoleFontPath"))->ToString())));
        }
        
        ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
    }
    
    void OpenWindow(int width, int height, bool fullscreen, bool openGL3Context) {
        Logger::begin("Window", Logger::LogLevel_Log) << "Loading OpenGL : Init Window/Context" << Logger::end();
        
        if (openGL3Context) {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            
            isGL3Context = true;
        } else {
            isGL3Context = false;
        }
        
        if (Config::GetBoolean("cl_aa")) {
            glfwWindowHint(GLFW_SAMPLES, 4);
        }
        
		window = glfwCreateWindow(width, height, "Engine2D", fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL); // you can resize how ever much you like
        
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
        
        glGetError();
        
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
        
        Draw2D::CheckGLError("PostSetupContext");
        
        Logger::begin("Window", Logger::LogLevel_Log) << "Loaded OpenGL" << Logger::end();
        
        InitFonts();
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
	
    void loadFont(std::string prettyName, std::string filename) {
        if (isGL3Context) {
            return;
        }
        
        Logger::begin("Font", Logger::LogLevel_Log)
            << "Loading Font: " << filename << " as " << prettyName
            << Logger::end();
        
        Draw2D::CheckGLError("PreLoadFont");
        
		if (Filesystem::FileExists(filename)) {
            ResourceManager::Load(filename);
            _fonts[prettyName] = new ResourceManager::FontResource(filename);
		} else {
			Logger::begin("Font", Logger::LogLevel_Error) << "Could not load font" << Logger::end();
		}
        
        Draw2D::CheckGLError("PostLoadFont");
        
        Logger::begin("Font", Logger::LogLevel_Log) << "Loaded Font" << Logger::end();
    }
    
	void InitFonts() {
        if (isGL3Context) {
            Logger::begin("Font", Logger::LogLevel_Warning) << "Skip loading fonts on OpenGL3.x" << Logger::end();
            return;
        }
        
        Profiler::Begin("LoadFonts");
            loadFont("basic", Config::GetString("cl_fontPath"));
            loadFont("monospace", Config::GetString("cl_consoleFontPath"));
        Profiler::End("LoadFonts");
	}
	
	void ShutdownFonts() {
        _fonts.clear();
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
    
    int getCommandLineArgCount() {
        return _argc;
    }
    
    const char** getCommandLineArgs() {
        return _argv;
    }
	
	void setDrawFunction(v8::Persistent<v8::Function> func) {
		_drawFunc = func;
	}
    
    void setKeyFunction(v8::Persistent<v8::Function> func) {
        _keyboardFunc = func;
    }
    
    void setPostLoadFunction(v8::Persistent<v8::Function> func) {
        _onPostLoadFunc = func;
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
    
    void runFile(std::string path, bool persist) {
        if (!Filesystem::FileExists(path)) {
            Logger::begin("Scripting", Logger::LogLevel_Error) << path << " Not Found" << Logger::end();
        } else {
            if (!_runFile(path, persist)) {
                Logger::begin("Scripting", Logger::LogLevel_Error) << "Could not load File : " << path << Logger::end();
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
        
        FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, width, height, 3 * width,
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
            
            Profiler::Begin("Frame");
            
            UpdateMousePos();
            
            Profiler::Begin("Draw");
            
            Draw2D::CheckGLError("startOfRendering");
            
            Profiler::Begin("glClear");
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Profiler::End("glClear");
            
            Draw2D::Begin2d();
            
            if (!_drawFunc.IsEmpty() && Config::GetBoolean("cl_scriptedDraw")) {
                Profiler::Begin("JSDraw", 0.04);
                if (!CallFunction(_drawFunc)) {
                    _drawFunc.Clear();
                }
                Profiler::End("JSDraw");
            }
            
            Profiler::Begin("EngineUI", 0.04);
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
        LoadBasicConfigs(_developerMode);
        
        Logger::begin("Application", Logger::LogLevel_Log) << "Starting" << Logger::end();
        
		Filesystem::Init(argv[0]);
        
        _argc = argc;
        _argv = argv;
        
        Profiler::Begin("InitScripting");
            InitScripting(_developerMode);
        Profiler::End("InitScripting");
        
        Profiler::Begin("InitOpenGL");
            InitOpenGL();
        Profiler::End("InitOpenGL");
        
        engine::EnableGLContext();
        
        if (!_onPostLoadFunc.IsEmpty()) {
            CallFunction(_onPostLoadFunc);
        }
        
        FreeImage_Initialise();
        
        UpdateScreen();
        
        Logger::begin("Application", Logger::LogLevel_Log) << "Loaded" << Logger::end();
        
        MainLoop();
        
        engine::DisableGLContext();
        
		ShutdownOpenGL();
		ShutdownScripting();

		Filesystem::Destroy();

		return 0;
	}
	
} // namespace Engine

int main(int argc, char const *argv[])
{
	return Engine::main(argc, argv);
}
