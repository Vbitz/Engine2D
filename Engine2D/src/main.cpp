#include "main.hpp"
	
namespace Engine {
	
	// state
	bool running = false;
	
	double lastTime = 0;
	
	int _screenWidth = 0;
	int _screenHeight = 0;
	
	GLFT_Font _font;
	
	std::vector<GLuint> _textures;
	
	std::map<std::string, std::string> _persists;

	// v8 Scripting
	v8::Isolate* _globalIsolate;

	v8::Persistent<v8::Context> _globalContext;
	
	v8::Persistent<v8::Function> _drawFunc;
	v8::Persistent<v8::Function> _keyboardFunc;

	std::map<std::string, long> _loadedFiles;
    
    std::string _keys[] = {"ESC","F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12",
                    "F13","F14","F15","F16","F17","F18","F19","F20","F21","F22","F23",
                    "F24","F25","UP","DOWN","LEFT","RIGHT","LSHIFT","RSHIFT","LCTRL",
                    "RCTRL","LALT","RALT","TAB","ENTER","BACKSPACE","INSERT","DEL",
                    "PAGEUP","PAGEDOWN","HOME","END","KP_0","KP_1","KP_2","KP_3",
                    "KP_4","KP_5","KP_6","KP_7","KP_8","KP_9","KP_DIVIDE","KP_MULTIPLY",
                    "KP_SUBTRACT","KP_ADD","KP_DECIMAL","KP_EQUAL","KP_ENTER","KP_NUM_LOCK",
                    "CAPS_LOCK","SCROLL_LOCK","PAUSE","LSUPER","RSUPER","MENU"};
	
	/****************************************
	 ****************************************
	 **************************************** 
	
		Scripting Defines
		=================
		I should really have each file return it's own table
	
	 ****************************************
	 ****************************************
	 ****************************************/
	
	#define addItem(table, js_name, funct) table->Set(js_name, v8::FunctionTemplate::New(funct)) 
	
	void InitScripting() {
		_globalIsolate = v8::Isolate::New();
		_globalIsolate->Enter();

		v8::HandleScope handle_scope;

		v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
	
		// sysTable
		v8::Handle<v8::ObjectTemplate> sysTable = v8::ObjectTemplate::New();
	
			addItem(sysTable, "println", JsSys::Println);
			addItem(sysTable, "runFile", JsSys::RunFile);
	
			addItem(sysTable, "drawFunc", JsDraw::Drawfunc);
            addItem(sysTable, "keyboardFunc", JsSys::KeyboardFunc);
        
            addItem(sysTable, "getGLVersion", JsSys::GetGLVersion);
        
            addItem(sysTable, "microtime", JsSys::Microtime);
            addItem(sysTable, "heapStats", JsSys::HeapStats);
        
        sysTable->Set("platform", v8::String::New(_PLATFORM));
	
		global->Set("sys", sysTable);
	
		// drawTable
		v8::Handle<v8::ObjectTemplate> drawTable = v8::ObjectTemplate::New();
	
			addItem(drawTable, "begin", JsDraw::Begin);
			addItem(drawTable, "end", JsDraw::End);
			addItem(drawTable, "rect", JsDraw::Rect);
			addItem(drawTable, "grid", JsDraw::Grid);
			addItem(drawTable, "grad", JsDraw::Grad);
			addItem(drawTable, "draw", JsDraw::Draw);
			addItem(drawTable, "setColorF", JsDraw::SetColorF);
			addItem(drawTable, "setColor", JsDraw::SetColor);
			addItem(drawTable, "setColorI", JsDraw::SetColorI);
			addItem(drawTable, "clearColor", JsDraw::ClearColor);
			addItem(drawTable, "print", JsDraw::Print);
			addItem(drawTable, "openImage", JsDraw::OpenImage);
        
            addItem(drawTable, "cameraReset", JsDraw::CameraReset);
            addItem(drawTable, "cameraPan", JsDraw::CameraPan);
            addItem(drawTable, "cameraZoom", JsDraw::CameraZoom);
            addItem(drawTable, "cameraRotate", JsDraw::CameraRotate);
        
            addItem(drawTable, "setDrawOffscreen", JsDraw::SetDrawOffscreen);
            addItem(drawTable, "getVerts", JsDraw::GetVerts);
            addItem(drawTable, "setCenter", JsDraw::SetCenter);
	
		global->Set("draw", drawTable);
	
		// inputTable
		v8::Handle<v8::ObjectTemplate> inputTable = v8::ObjectTemplate::New();
	
            addItem(inputTable, "keyDown", JsInput::KeyDown);
        
		global->Set("input", inputTable);

		_globalContext = v8::Context::New(NULL, global);

		runFile("lib/boot.js", true);
	}
	
	#undef addItem
	
	void ShutdownScripting() {
		_globalIsolate->Exit();
		_globalIsolate->Dispose();

		for (int i = 0; i < _textures.size(); ++i)
		{
			glDeleteTextures(1, &_textures[i]);
		}
	
		_textures.clear();
	}
	
	void CallFunction(v8::Handle<v8::Value> func) {
		v8::HandleScope scp;
		v8::Context::Scope ctx_scope(_globalContext);
	
		v8::Handle<v8::Function> real_func = v8::Handle<v8::Function>::Cast(func);
	
		real_func->Call(_globalContext->Global(), 0, NULL);
	}
	
	void UpdateMousePos() {
		int x = 0;
		int y = 0;
		glfwGetMousePos(&x, &y);
	
		v8::HandleScope scp;
		v8::Context::Scope ctx_scope(_globalContext);
	
		v8::Local<v8::Object> obj = v8::Context::GetCurrent()->Global();
		v8::Local<v8::Object> input_table = v8::Object::Cast(*obj->Get(v8::String::New("input")));
		input_table->Set(v8::String::New("mouseX"), v8::Number::New(x));
		input_table->Set(v8::String::New("mouseY"), v8::Number::New(y));
		input_table->Set(v8::String::New("leftMouseButton"), v8::Boolean::New(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS));
		input_table->Set(v8::String::New("rightMouseButton"), v8::Boolean::New(glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS));
	}
	
	void UpdateScreen() {
		v8::HandleScope scp;
		v8::Context::Scope ctx_scope(_globalContext);
	
		v8::Local<v8::Object> obj = v8::Context::GetCurrent()->Global();
		v8::Local<v8::Object> input_table = v8::Object::Cast(*obj->Get(v8::String::New("sys")));
		input_table->Set(v8::String::New("screenWidth"), v8::Number::New(_screenWidth));
		input_table->Set(v8::String::New("screenHeight"), v8::Number::New(_screenHeight));
	}
	
	void ResizeWindow(int w, int h) {
		_screenWidth = w;
		_screenHeight = h;
		if (running)
		{
			UpdateScreen();
		}
		glViewport(0, 0, w, h);
	}
	
	void CharPress(int key, int state) {
        if (_keyboardFunc.IsEmpty()) {
            return;
        }
            
        if (key > 256) {
            return; // that's handled by KeyPress
        }
	
        v8::HandleScope scp;
        v8::Context::Scope ctx_scope(_globalContext);
	
        v8::Handle<v8::Function> real_func = v8::Handle<v8::Function>::Cast(_keyboardFunc);
	
        uint16_t str[2] = {(uint16_t)key, 0x00};

        v8::Handle<v8::Value> argv[3];
	
        argv[0] = v8::String::NewSymbol("char");
        argv[1] = v8::String::New(str);
        argv[2] = v8::Boolean::New(state);
	
        real_func->Call(_globalContext->Global(), 3, argv);
	}
	
	void KeyPress(int key, int state) {
        if (_keyboardFunc.IsEmpty()) {
            return;
        }
        
        if (key < 256) {
            return; // that's already handled by CharPress
        }
	
        v8::HandleScope scp;
        v8::Context::Scope ctx_scope(_globalContext);
        
        v8::Handle<v8::Function> real_func = v8::Handle<v8::Function>::Cast(_keyboardFunc);

        v8::Handle<v8::Value> argv[3];
        
        argv[0] = v8::String::NewSymbol("special");
        argv[1] = v8::String::NewSymbol(_keys[key - 257].c_str());
        argv[2] = v8::Boolean::New(state == 1);
        
        real_func->Call(_globalContext->Global(), 3, argv);
	}
	
	void InitOpenGL() {
		glfwInit();
	
		glfwOpenWindow(800, 600, 1, 1, 1, 1, 1, 1, GLFW_WINDOW); // you can resize how ever much you like
	
		glfwSetWindowSizeCallback(ResizeWindow);
		glfwSetKeyCallback(KeyPress);
		glfwSetCharCallback(CharPress);
	
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	
	void ShutdownOpenGL() {
		glfwTerminate();
	}
	
	// font rendering
	
	void InitFonts() {
		std::string fontPath = Filesystem::GetRealPath("fonts/OpenSans-Regular.ttf");
		if (fontPath.length() > 0) {
			_font.open(fontPath.c_str(), 16);
		} else {
			std::cout << "Could not load font" << std::endl;
		}
	}
	
	void ShutdownFonts() {
	
	}
	
	// semi-realtime time loading
	
	void CheckUpdate() {
		typedef std::map<std::string, long>::iterator it_type;
		for(it_type iterator = _loadedFiles.begin(); iterator != _loadedFiles.end(); iterator++) {
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
	
	void addTexture(int tex) {
		_textures.push_back(tex);
	}
	
	GLFT_Font* getFont() {
		return &_font;
	}
	
	void setDrawFunction(v8::Persistent<v8::Function> func) {
		_drawFunc = func;
	}
    
    void setKeyFunction(v8::Persistent<v8::Function> func) {
        _keyboardFunc = func;
    }
    
	bool _runFile(std::string path, bool persist) {
        v8::HandleScope handle_scope;
        
		std::cout << "Loading File: " << path << std::endl;

		v8::Context::Scope ctx_scope(_globalContext);

		std::string inputScript = Filesystem::GetFileContentString(path);

		v8::TryCatch tryCatch;

		v8::Handle<v8::Script> script = v8::Script::Compile(
			v8::String::New(inputScript.c_str()));

		if (script.IsEmpty()) {
			std::cout << "Could not Load file" << std::endl;
			v8::Handle<v8::Value> exception = tryCatch.Exception();
			v8::String::AsciiValue exception_str(exception);
			printf("Exception: %s\n", *exception_str);
            return false;
		} else {
			script->Run();
            std::cout << "Loaded File: " << path << std::endl;
            if (persist) {
                _loadedFiles[path] = Filesystem::GetFileModifyTime(path);
            }
            return true;
		}
	}
    
    void runFile(std::string path, bool persist) {
        while (!_runFile(path, persist)) {
            std::cout << "Could not load File" << std::endl;
        }
    }
	
	// main function
	
	int main(int argc, char const *argv[]) {
		Filesystem::Init(argv[0]);
		
		// TODO: split OpenGl, Scripting and Fonts into seprate files
		InitOpenGL();
		InitScripting();
		InitFonts();
	
		running = true;
	
		UpdateScreen();
	
		while (running) {
			if (!glfwGetWindowParam(GLFW_ACTIVE)) {
				glfwWaitEvents();
                sleep(0);
				continue;
			}
            
            CheckUpdate();
            
            UpdateMousePos();
	
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
			JsDraw::Begin2d();
	
			if (!_drawFunc.IsEmpty()) {
				CallFunction(_drawFunc);
			}
	
			JsDraw::End2d();
	
			glfwSwapBuffers();
	
			if (!glfwGetWindowParam(GLFW_OPENED))
				running = false;
		}
	
		ShutdownOpenGL();
		ShutdownScripting();
		ShutdownFonts();

		Filesystem::Destroy();

		return 0;
	}
	
} // namespace Engine

int main(int argc, char const *argv[])
{
	return Engine::main(argc, argv);
}