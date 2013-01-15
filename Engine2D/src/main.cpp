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
	
	long lastUpdate = 0;
	
	// v8 Scripting
	v8::Persistent<v8::Context> _globalContext;
	
	v8::Persistent<v8::Function> _drawFunc;
	v8::Persistent<v8::Function> _keyboardFunc;
	
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
		v8::HandleScope scp;
	
		v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
	
		// sysTable
		v8::Handle<v8::ObjectTemplate> sysTable = v8::ObjectTemplate::New();
	
			addItem(sysTable, "println", JsSys::Println);
	
			addItem(sysTable, "drawFunc", JsDraw::Drawfunc);
	
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
	
		global->Set("draw", drawTable);
	
		// inputTable
		v8::Handle<v8::ObjectTemplate> inputTable = v8::ObjectTemplate::New();
	
		global->Set("input", inputTable);
	
		_globalContext = v8::Context::New(NULL, global);
	
		v8::Context::Scope ctx_scope(_globalContext);
	
		std::string inputScript = Filesystem::GetFileContentString("script/startup.js");

		v8::Handle<v8::Script> script = v8::Script::Compile(
			v8::String::New(inputScript.c_str()));
		script->Run();
	}
	
	#undef addItem
	
	void ShutdownScripting() {
	
		_globalContext.Dispose();
	
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
	
	void UpdateFrameTime() {
		v8::HandleScope scp;
		v8::Context::Scope ctx_scope(_globalContext);
	
		v8::Local<v8::Object> obj = v8::Context::GetCurrent()->Global();
		v8::Local<v8::Object> input_table = v8::Object::Cast(*obj->Get(v8::String::New("sys")));
		input_table->Set(v8::String::New("frameTime"), v8::Number::New(glfwGetTime() - lastTime));
		lastTime = glfwGetTime(); // eh
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
		if (state == GLFW_PRESS) {
			if (_keyboardFunc.IsEmpty()) {
				return;
			}
	
			v8::HandleScope scp;
			v8::Context::Scope ctx_scope(_globalContext);
	
			v8::Handle<v8::Function> real_func = v8::Handle<v8::Function>::Cast(_keyboardFunc);
	
			uint16_t str[2] = {(uint16_t)key, 0x00};
	
			v8::Handle<v8::Value> argv[2];
	
			argv[0] = v8::String::New(str);
			argv[1] = v8::Undefined();
	
			real_func->Call(_globalContext->Global(), 2, argv);
		}
	}
	
	void KeyPress(int key, int state) {
		if (state == GLFW_PRESS) {
			if (_keyboardFunc.IsEmpty()) {
				return;
			}
	
			v8::HandleScope scp;
			v8::Context::Scope ctx_scope(_globalContext);
	
			v8::Handle<v8::Function> real_func = v8::Handle<v8::Function>::Cast(_keyboardFunc);
	
			const char* str = "";
	
			if (key == GLFW_KEY_BACKSPACE)
			{
				str = "backspace";
			} else if (key == GLFW_KEY_UP) {
				str = "up";
			} else if (key == GLFW_KEY_DOWN) {
				str = "down";
			} else if (key == GLFW_KEY_LEFT) {
				str = "left";
			} else if (key == GLFW_KEY_RIGHT) {
				str = "right";
			} else if (key == GLFW_KEY_ENTER) {
				str = "enter";
			}
	
			v8::Handle<v8::Value> argv[2];
		
			argv[0] = v8::Undefined();
			argv[1] = v8::String::New(str);
	
			real_func->Call(_globalContext->Global(), 2, argv);
		}
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
		_font.open(Filesystem::GetRealPath("fonts/SourceSansPro-Regular.ttf").c_str(), 16);
	}
	
	void ShutdownFonts() {
	
	}
	
	// semi-realtime time loading
	
	bool CheckUpdate() {
		long lastMod = Filesystem::GetFileModifyTime("script/startup.js");
		if (lastMod > lastUpdate) {
			lastUpdate = lastMod;
			return true;
		} else {
			return false;
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
	
	// main function
	
	int main(int argc, char const *argv[]) {
		Filesystem::Init(argv[0]);

		lastUpdate = Filesystem::GetFileModifyTime("script/startup.js");
		
		// TODO: split OpenGl, Scripting and Fonts into seprate files
		InitOpenGL();
		InitScripting();
		InitFonts();
	
		running = true;
	
		UpdateScreen();
	
		while (running) {
			if (!glfwGetWindowParam(GLFW_ACTIVE)) {
				glfwWaitEvents();
#ifdef _WIN32
				Sleep(0);
#else
				sleep(0);
#endif
				continue;
			}
	
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
			if (CheckUpdate()) {
				ShutdownScripting();
				InitScripting();
				UpdateScreen();
			}
	
			JsDraw::Begin2d();
	
			if (!_drawFunc.IsEmpty()) {
				CallFunction(_drawFunc);
			}
	
			JsDraw::End2d();
	
			UpdateMousePos();
			UpdateFrameTime();
	
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