#include "JSDraw.hpp"

bool _effDraw = false;

double _currentColorR = 1.0f;
double _currentColorG = 1.0f;
double _currentColorB = 1.0f;

void SetColor() {
	glColor3f(_currentColorR, _currentColorG, _currentColorB);
}

void Begin2d() {
	_effDraw = false;
	glPushMatrix();
	glDisable(GL_DEPTH_TEST);
	glLoadIdentity();
	glOrtho(0, getScreenWidth(), getScreenHeight(), 0, 1, -1);
}

void End2d() {
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
}

v8::Handle<v8::Value> JS_Draw_Drawfunc(const v8::Arguments& args) {
	if (args.Length() != 1) {
		// TODO : Error
		return v8::Undefined();
	}

	if (!args[0]->IsFunction())
	{
		// TODO : Error
		return v8::Undefined();
	}

	v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args[0]);

	setDrawFunction(v8::Persistent<v8::Function>::New(func));

	return v8::Undefined();
}

v8::Handle<v8::Value> JS_Draw_Begin(const v8::Arguments& args) {
	if (_effDraw) {
		// TODO : Error
		return v8::Undefined();
	}

	glBegin(GL_QUADS);

	_effDraw = true;

	return v8::Undefined();
}

v8::Handle<v8::Value> JS_Draw_End(const v8::Arguments& args) {
	if (!_effDraw) {
		// TODO : Error
		return v8::Undefined();
	}

	glEnd();

	_effDraw = false;

	return v8::Undefined();
}

v8::Handle<v8::Value> JS_Draw_Rect(const v8::Arguments& args) {
	GLfloat x, y, w, h;

	if (args.Length() != 4) {
		// TODO : Error
		return v8::Undefined();
	}

	if (!args[0]->IsNumber() && !args[1]->IsNumber() && !args[2]->IsNumber() && !args[3]->IsNumber()) {
		// TODO : Error
		return v8::Undefined();
	}

	x = (GLfloat)args[0]->NumberValue();
	y = (GLfloat)args[1]->NumberValue();
	w = (GLfloat)args[2]->NumberValue();
	h = (GLfloat)args[3]->NumberValue();

	if (!_effDraw) {
		glBegin(GL_QUADS);
	}
		SetColor();
		glVertex3f(x, y, 0);
		glVertex3f(x + w, y, 0);
		glVertex3f(x + w, y + h, 0);
		glVertex3f(x, y + h, 0);
	if (!_effDraw) {
		glEnd();
	}

	return v8::Undefined();
}

v8::Handle<v8::Value> JS_Draw_Grid(const v8::Arguments& args) {
	GLfloat x, y, w, h;

	if (args.Length() != 4) {
		// TODO : Error
		return v8::Undefined();
	}

	if (!args[0]->IsNumber() && !args[1]->IsNumber() && !args[2]->IsNumber() && !args[3]->IsNumber()) {
		// TODO : Error
		return v8::Undefined();
	}

	x = (GLfloat)args[0]->NumberValue();
	y = (GLfloat)args[1]->NumberValue();
	w = (GLfloat)args[2]->NumberValue();
	h = (GLfloat)args[3]->NumberValue();

	if (_effDraw) {
		glEnd();
	}
		SetColor();
		glBegin(GL_LINE_LOOP);
		glVertex3f(x, y, 0);
		glVertex3f(x + w, y, 0);
		glVertex3f(x + w, y + h, 0);
		glVertex3f(x, y + h, 0);
		glEnd();
	if (_effDraw) {
		glBegin(GL_QUADS);
	}

	return v8::Undefined();
}

v8::Handle<v8::Value> JS_Draw_Grad(const v8::Arguments& args) {
	GLfloat x, y, w, h;
	unsigned int col1, col2;
	bool vert;

	if (args.Length() != 7) {
		// TODO : Error
		return v8::Undefined();
	}

	if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber() ||
			!args[4]->IsNumber() || !args[5]->IsNumber() || !args[6]->IsBoolean()) {
		// TODO : Error
		return v8::Undefined();
	}

	x = (GLfloat)args[0]->NumberValue();
	y = (GLfloat)args[1]->NumberValue();
	w = (GLfloat)args[2]->NumberValue();
	h = (GLfloat)args[3]->NumberValue();

	col1 = (unsigned int)args[4]->NumberValue();
	col2 = (unsigned int)args[5]->NumberValue();

	vert = args[6]->BooleanValue();

	if (col1 > 255 * 255 * 255)
	{
		// TODO : Error
		return v8::Undefined();
	}

	if (col2 > 255 * 255 * 255)
	{
		// TODO : Error
		return v8::Undefined();
	}

	if (!_effDraw) {
		glBegin(GL_QUADS);
	}
		if (vert) {
			glColor3f((float)((col1 & 0xff0000) >> 16) / 255, (float)((col1 & 0x00ff00) >> 8) / 255, (float)(col1 & 0x0000ff) / 255);
			glVertex3f(x, y, 0);
			glVertex3f(x + w, y, 0);
			glColor3f((float)((col2 & 0xff0000) >> 16) / 255, (float)((col2 & 0x00ff00) >> 8) / 255, (float)(col2 & 0x0000ff) / 255);
			glVertex3f(x + w, y + h, 0);
			glVertex3f(x, y + h, 0);
		} else {
			glColor3f((float)((col1 & 0xff0000) >> 16) / 255, (float)((col1 & 0x00ff00) >> 8) / 255, (float)(col1 & 0x0000ff) / 255);
			glVertex3f(x, y, 0);
			glColor3f((float)((col2 & 0xff0000) >> 16) / 255, (float)((col2 & 0x00ff00) >> 8) / 255, (float)(col2 & 0x0000ff) / 255);
			glVertex3f(x + w, y, 0);
			glVertex3f(x + w, y + h, 0);
			glColor3f((float)((col1 & 0xff0000) >> 16) / 255, (float)((col1 & 0x00ff00) >> 8) / 255, (float)(col1 & 0x0000ff) / 255);
			glVertex3f(x, y + h, 0);
		}
	if (!_effDraw) {
		glEnd();
	}

	return v8::Undefined();
}

v8::Handle<v8::Value> JS_Draw_Draw(const v8::Arguments& args) {
	GLuint texID;
	GLfloat x, y, w, h;

	if (args.Length() != 5) {
		// TODO : Error
		return v8::Undefined();
	}

	if (!args[0]->IsNumber() && !args[1]->IsNumber() && !args[2]->IsNumber() && !args[3]->IsNumber() && !args[4]->IsNumber()) {
		// TODO : Error
		return v8::Undefined();
	}

	texID = (GLuint)args[0]->NumberValue();
	x = (GLfloat)args[1]->NumberValue();
	y = (GLfloat)args[2]->NumberValue();
	w = (GLfloat)args[3]->NumberValue();
	h = (GLfloat)args[4]->NumberValue();

	if (_effDraw) {
		glEnd();
	}

	glEnable(GL_TEXTURE_2D);
	//glDisable(GL_BLEND);

	glBindTexture(GL_TEXTURE_2D, texID); 

	glBegin(GL_QUADS);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y, 0);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + w, y, 0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + w, y + h, 0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y + h, 0);

	glEnd();

	//glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);

	if (_effDraw) {
		glBegin(GL_QUADS);
	}

	return v8::Undefined();
}

v8::Handle<v8::Value> JS_Draw_SetColorF(const v8::Arguments& args) {
	if (args.Length() != 3) {
		// TODO : Error
		return v8::Undefined();
	}

	if (!args[0]->IsNumber() && !args[1]->IsNumber() && !args[2]->IsNumber()) {
		// TODO : Error
		return v8::Undefined();
	}

	_currentColorR = args[0]->NumberValue();
	_currentColorG = args[1]->NumberValue();
	_currentColorB = args[2]->NumberValue();

	return v8::Undefined();
}

v8::Handle<v8::Value> JS_Draw_SetColor(const v8::Arguments& args) {
	if (args.Length() != 1) {
		// TODO : Error
		return v8::Undefined();
	}

	if (!args[0]->IsNumber()) {
		// TODO : Error
		return v8::Undefined();
	}

	int col1 = args[0]->NumberValue();

	if (col1 > (256 * 256 * 256))
	{
		// TODO : Error
		return v8::Undefined();
	}

	_currentColorR = (float)((col1 & 0xff0000) >> 16) / 255;
	_currentColorG = (float)((col1 & 0x00ff00) >> 8) / 255;
	_currentColorB = (float)(col1 & 0x0000ff) / 255;

	return v8::Undefined();
}

v8::Handle<v8::Value> JS_Draw_SetColorI(const v8::Arguments& args) {
	if (args.Length() != 3) {
		// TODO : Error
		return v8::Undefined();
	}

	if (!args[0]->IsNumber() && !args[1]->IsNumber() && !args[2]->IsNumber()) {
		// TODO : Error
		return v8::Undefined();
	}

	_currentColorR = args[0]->NumberValue() / 255;
	_currentColorG = args[1]->NumberValue() / 255;
	_currentColorB = args[2]->NumberValue() / 255;

	return v8::Undefined();
}

v8::Handle<v8::Value> JS_Draw_ClearColor(const v8::Arguments& args) {
	if (args.Length() != 3) {
		// TODO : Error
		return v8::Undefined();
	}

	if (!args[0]->IsNumber() && !args[1]->IsNumber() && !args[2]->IsNumber()) {
		// TODO : Error
		return v8::Undefined();
	}

	glClearColor(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue(), 1.0f);

	return v8::Undefined();
}

v8::Handle<v8::Value> JS_Draw_Print(const v8::Arguments& args) {
	if (args.Length() != 3) {
		// TODO : Error
		return v8::Undefined();
	}

	if (!args[0]->IsNumber() && !args[1]->IsNumber() && !args[2]->IsString()) {
		// TODO : Error
		return v8::Undefined();
	}

	if (_effDraw)
	{
		glEnd();
	}

	glEnable(GL_TEXTURE_2D);
	SetColor();
	getFont()->beginDraw(args[0]->NumberValue(), args[1]->NumberValue()) << *v8::String::Utf8Value(args[2]) << getFont()->endDraw();
	glDisable(GL_TEXTURE_2D);

	if (_effDraw)
	{
		glBegin(GL_QUADS);
	}

	return v8::Undefined();
}

// add suport for this function to open anything ( or just png\jpg\gif )
v8::Handle<v8::Value> JS_Draw_OpenImage(const v8::Arguments& args) {
	if (args.Length() != 1) {
		// TODO : Error
		return v8::Undefined();
	}

	if (!args[0]->IsString()) {
		// TODO : Error
		return v8::Undefined();
	}

	// check if the file exists and is a tga image

	GLuint text = 0;

	glGenTextures(1, &text);

	glBindTexture(GL_TEXTURE_2D, text);

	glfwLoadTexture2D(*v8::String::AsciiValue(args[0]), GLFW_BUILD_MIPMAPS_BIT);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
						GL_LINEAR_MIPMAP_LINEAR );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
						GL_LINEAR );

	addTexture(text);

	return v8::Integer::New(text);
}