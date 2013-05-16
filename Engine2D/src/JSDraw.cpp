#include "JSDraw.hpp"
		
namespace Engine {

	namespace JsDraw {
        bool _drawOffScreen = true;
		
		double _currentColorR = 1.0f;
		double _currentColorG = 1.0f;
		double _currentColorB = 1.0f;
        
        int _polygons = 0;
        
        int _centerX = 0;
        int _centerY = 0;
		
		void SetColor() {
			glColor3f(_currentColorR, _currentColorG, _currentColorB);
		}
        
        void resetMatrix() {
            glPushMatrix();
			glDisable(GL_DEPTH_TEST);
			glLoadIdentity();
			glOrtho(0, getScreenWidth(), getScreenHeight(), 0, 1, -1);
        }
        
        bool isOffscreen(int x, int y, int w, int h) {
            return false;
        }
		
		void Begin2d() {
            resetMatrix();
		}
		
		void End2d() {
			glEnable(GL_DEPTH_TEST);
			glPopMatrix();
            
            _polygons = 0;
		}
		
		v8::Handle<v8::Value> Rect(const v8::Arguments& args) {
			v8::HandleScope scope;

			GLfloat x, y, w, h;
		
			if (args.Length() != 4) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			if (!args[0]->IsNumber() && !args[1]->IsNumber() && !args[2]->IsNumber() && !args[3]->IsNumber()) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			x = (GLfloat)args[0]->NumberValue();
			y = (GLfloat)args[1]->NumberValue();
			w = (GLfloat)args[2]->NumberValue();
			h = (GLfloat)args[3]->NumberValue();
            
            glBegin(GL_QUADS);
				SetColor();
				glVertex3f(x        - _centerX, y       - _centerY, 0);
				glVertex3f(x + w    - _centerX, y       - _centerY, 0);
				glVertex3f(x + w    - _centerX, y + h   - _centerY, 0);
				glVertex3f(x        - _centerX, y + h   - _centerY, 0);
            glEnd();
            
            _polygons += 4;
		
			return scope.Close(v8::Undefined());
		}
		
		v8::Handle<v8::Value> Grid(const v8::Arguments& args) {
			v8::HandleScope scope;

			GLfloat x, y, w, h;
		
			if (args.Length() != 4) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			if (!args[0]->IsNumber() && !args[1]->IsNumber() && !args[2]->IsNumber() && !args[3]->IsNumber()) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			x = (GLfloat)args[0]->NumberValue();
			y = (GLfloat)args[1]->NumberValue();
			w = (GLfloat)args[2]->NumberValue();
			h = (GLfloat)args[3]->NumberValue();
            
            SetColor();
            glBegin(GL_LINE_LOOP);
                glVertex3f(x        - _centerX, y       - _centerY, 0);
                glVertex3f(x + w    - _centerX, y       - _centerY, 0);
                glVertex3f(x + w    - _centerX, y + h   - _centerY, 0);
                glVertex3f(x        - _centerX, y + h   - _centerY, 0);
				glVertex3f(x        - _centerX, y       - _centerY, 0);
            glEnd();
		
            _polygons += 5;
            
			return scope.Close(v8::Undefined());
		}
		
		v8::Handle<v8::Value> Grad(const v8::Arguments& args) {
			v8::HandleScope scope;

			GLfloat x, y, w, h;
			unsigned int col1, col2;
			bool vert;
		
			if (args.Length() != 7) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber() ||
					!args[4]->IsNumber() || !args[5]->IsNumber() || !args[6]->IsBoolean()) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			x = (GLfloat)args[0]->NumberValue();
			y = (GLfloat)args[1]->NumberValue();
			w = (GLfloat)args[2]->NumberValue();
			h = (GLfloat)args[3]->NumberValue();
            
            if (isOffscreen(x, y, w, h)) {
                return scope.Close(v8::Undefined());
            }
		
			col1 = (unsigned int)args[4]->NumberValue();
			col2 = (unsigned int)args[5]->NumberValue();
		
			vert = args[6]->BooleanValue();
		
			if (col1 > 256 * 256 * 256) // there fixed that age old bug
			{
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			if (col2 > 256 * 256 * 256)
			{
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
            
            glBegin(GL_QUADS);
				if (vert) {
					glColor3f((float)((col1 & 0xff0000) >> 16) / 255, (float)((col1 & 0x00ff00) >> 8) / 255, (float)(col1 & 0x0000ff) / 255);
                    glVertex3f(x        - _centerX, y       - _centerY, 0);
                    glVertex3f(x + w    - _centerX, y       - _centerY, 0);
					glColor3f((float)((col2 & 0xff0000) >> 16) / 255, (float)((col2 & 0x00ff00) >> 8) / 255, (float)(col2 & 0x0000ff) / 255);
                    glVertex3f(x + w    - _centerX, y + h   - _centerY, 0);
                    glVertex3f(x        - _centerX, y + h   - _centerY, 0);
				} else {
					glColor3f((float)((col1 & 0xff0000) >> 16) / 255, (float)((col1 & 0x00ff00) >> 8) / 255, (float)(col1 & 0x0000ff) / 255);
                    glVertex3f(x        - _centerX, y       - _centerY, 0);
					glColor3f((float)((col2 & 0xff0000) >> 16) / 255, (float)((col2 & 0x00ff00) >> 8) / 255, (float)(col2 & 0x0000ff) / 255);
                    glVertex3f(x + w    - _centerX, y       - _centerY, 0);
                    glVertex3f(x + w    - _centerX, y + h   - _centerY, 0);
					glColor3f((float)((col1 & 0xff0000) >> 16) / 255, (float)((col1 & 0x00ff00) >> 8) / 255, (float)(col1 & 0x0000ff) / 255);
                    glVertex3f(x        - _centerX, y + h   - _centerY, 0);
				}
            glEnd();
            
            _polygons += 4;
		
			return scope.Close(v8::Undefined());
		}
		
		v8::Handle<v8::Value> Draw(const v8::Arguments& args) {
			v8::HandleScope scope;

			GLuint texID;
			GLfloat x, y, w, h;
		
			if (args.Length() != 5) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			if (!args[0]->IsNumber() && !args[1]->IsNumber() && !args[2]->IsNumber() && !args[3]->IsNumber() && !args[4]->IsNumber()) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			texID = (GLuint)args[0]->NumberValue();
			x = (GLfloat)args[1]->NumberValue();
			y = (GLfloat)args[2]->NumberValue();
			w = (GLfloat)args[3]->NumberValue();
			h = (GLfloat)args[4]->NumberValue();
		
			glEnable(GL_TEXTURE_2D);
			//glDisable(GL_BLEND);
		
			glBindTexture(GL_TEXTURE_2D, texID); 
		
			glBegin(GL_QUADS);
		
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(x        - _centerX, y       - _centerY, 0);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(x + w    - _centerX, y       - _centerY, 0);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(x + w    - _centerX, y + h   - _centerY, 0);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(x        - _centerX, y + h   - _centerY, 0);

			glEnd();
            
            _polygons += 4;
		
			//glBindTexture(GL_TEXTURE_2D, 0);
		
			glDisable(GL_TEXTURE_2D);
		
			return scope.Close(v8::Undefined());
		}
		
		v8::Handle<v8::Value> SetColorF(const v8::Arguments& args) {
			v8::HandleScope scope;

			if (args.Length() != 3) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			if (!args[0]->IsNumber() && !args[1]->IsNumber() && !args[2]->IsNumber()) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			_currentColorR = args[0]->NumberValue();
			_currentColorG = args[1]->NumberValue();
			_currentColorB = args[2]->NumberValue();
		
			return scope.Close(v8::Undefined());
		}
		
		v8::Handle<v8::Value> SetColor(const v8::Arguments& args) {
			v8::HandleScope scope;

			if (args.Length() != 1) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			if (!args[0]->IsNumber()) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			int col1 = args[0]->NumberValue();
		
			if (col1 > (256 * 256 * 256))
			{
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			_currentColorR = (float)((col1 & 0xff0000) >> 16) / 255;
			_currentColorG = (float)((col1 & 0x00ff00) >> 8) / 255;
			_currentColorB = (float)(col1 & 0x0000ff) / 255;
		
			return scope.Close(v8::Undefined());
		}
		
		v8::Handle<v8::Value> SetColorI(const v8::Arguments& args) {
			v8::HandleScope scope;

			if (args.Length() != 3) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			if (!args[0]->IsNumber() && !args[1]->IsNumber() && !args[2]->IsNumber()) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			_currentColorR = args[0]->NumberValue() / 255;
			_currentColorG = args[1]->NumberValue() / 255;
			_currentColorB = args[2]->NumberValue() / 255;
		
			return scope.Close(v8::Undefined());
		}
		
		v8::Handle<v8::Value> ClearColor(const v8::Arguments& args) {
			v8::HandleScope scope;

			if (args.Length() != 3) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			if (!args[0]->IsNumber() && !args[1]->IsNumber() && !args[2]->IsNumber()) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			glClearColor(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue(), 1.0f);
		
			return scope.Close(v8::Undefined());
		}
		
		v8::Handle<v8::Value> Print(const v8::Arguments& args) {
			v8::HandleScope scope;

			if (args.Length() != 3) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			if (!args[0]->IsNumber() && !args[1]->IsNumber() && !args[2]->IsString()) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}

            const char* str = *v8::String::Utf8Value(args[2]);
		
			glEnable(GL_TEXTURE_2D);
			SetColor();
			getFont()->beginDraw(args[0]->NumberValue() - _centerX, args[1]->NumberValue() - _centerY) << str << getFont()->endDraw();
			glDisable(GL_TEXTURE_2D);
            
            _polygons += strlen(str) * 4;
		
			return scope.Close(v8::Undefined());
		}
		
		// TODO: add support for this function to open anything ( or just png\jpg\gif )
		v8::Handle<v8::Value> OpenImage(const v8::Arguments& args) {
			v8::HandleScope scope;

			if (args.Length() != 1) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			if (!args[0]->IsString()) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}

			if (!Filesystem::FileExists(*v8::String::AsciiValue(args[0]))) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
		
			GLuint text = 0;
		
			glGenTextures(1, &text);
		
			glBindTexture(GL_TEXTURE_2D, text);
		
			// TODO: Write my own loader

			glfwLoadTexture2D(Filesystem::GetRealPath(*v8::String::AsciiValue(args[0])).c_str(), GLFW_BUILD_MIPMAPS_BIT);
		
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
								GL_LINEAR_MIPMAP_LINEAR );
		
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
								GL_LINEAR );
		
			addTexture(text);
		
			return scope.Close(v8::Integer::New(text));
		}
        
        // basicly restarts 2d drawing
        v8::Handle<v8::Value> CameraReset(const v8::Arguments& args) {
            v8::HandleScope scope;
            
            int oldVertCount = _polygons;
            
            End2d();
            Begin2d();
            
            _polygons = oldVertCount;
            
            return scope.Close(v8::Undefined());
        }
        
        v8::Handle<v8::Value> CameraPan(const v8::Arguments& args) {
            v8::HandleScope scope;
            
			if (args.Length() != 2) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
            
			if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
            
            glTranslatef(args[0]->NumberValue(), args[1]->NumberValue(), 0.0f);
            
            return scope.Close(v8::Undefined());
        }
        
        v8::Handle<v8::Value> CameraZoom(const v8::Arguments& args) {
            v8::HandleScope scope;
            
            if (args.Length() != 1) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
            
			if (!args[0]->IsNumber()) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
            
            double zoomFactor = args[0]->NumberValue();
            
            glScalef(zoomFactor, zoomFactor, 0.0f);
            
            return scope.Close(v8::Undefined());
        }
        
        v8::Handle<v8::Value> CameraRotate(const v8::Arguments& args) {
            v8::HandleScope scope;
            
            if (args.Length() != 1) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
            
			if (!args[0]->IsNumber()) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
            
            double rotateFactor = args[0]->NumberValue();
            
            glRotatef(rotateFactor, 0.0f, 0.0f, 1.0f);
            
            return scope.Close(v8::Undefined());
        }
        
        v8::Handle<v8::Value> GetTextWidth(const v8::Arguments& args) {
            v8::HandleScope scope;
            
            float textWidth = args[0]->ToString()->Length() * 10;
            
            return scope.Close(v8::Number::New(textWidth));
        }
        
        v8::Handle<v8::Value> GetVerts(const v8::Arguments& args) {
            v8::HandleScope scope;
            
            return scope.Close(v8::Integer::New(_polygons));
        }
        
        v8::Handle<v8::Value> SetDrawOffscreen(const v8::Arguments& args) {
            v8::HandleScope scope;
            
            if (args.Length() != 1) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
            
			if (!args[0]->IsBoolean()) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
            
            _drawOffScreen = args[0]->BooleanValue();
            
            return scope.Close(v8::Undefined());
        }
        
        v8::Handle<v8::Value> SetCenter(const v8::Arguments& args) {
            v8::HandleScope scope;
            
            if (args.Length() != 2) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
            
			if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
				// TODO : Error
				return scope.Close(v8::Undefined());
			}
            
            _centerX = args[0]->Int32Value();
            _centerY = args[1]->Int32Value();
            
            return scope.Close(v8::Undefined());
        }

	} // namespace JsDraw
} // namespace Engine