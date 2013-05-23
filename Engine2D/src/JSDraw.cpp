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
        
        float clamp(float val, float min, float max) {
            if (val != val) {
                return 0.0f; // handle nan
            } else if (val < min) {
                return min;
            } else if (val > max) {
                return max;
            }
            return val;
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
        
        void beginRendering(GLenum mode) {
            glBegin(mode);
            glColor3f(_currentColorR, _currentColorG, _currentColorB);
        }
        
        void endRendering() {
            glEnd();
        }
        
        void addVert(float x, float y, float z) {
            glVertex3f(x - _centerX, y - _centerY, z);
            _polygons++;
        }
        
        void addVert(float x, float y, float z, float r, int g, int b) {
            glColor3f(r, g, b);
            addVert(x, y, z);
        }
        
        void addVert(float x, float y, float z, float s, float t) {
            glTexCoord2f(s, t);
            addVert(x, y, z);
        }
        
        void addVert(float x, float y, float z, float r, float g, float b, float s, float t) {
            glTexCoord2f(s, t);
            addVert(x, y, z, r, g, b);
        }
        
        void enableTexture(int texId) {
            glEnable(GL_TEXTURE_2D);
            
            glBindTexture(GL_TEXTURE_2D, texId);
        }
        
        void disableTexture() {
            glBindTexture(GL_TEXTURE_2D, 0);
            
            glDisable(GL_TEXTURE_2D);
        }
		
		void Begin2d() {
            resetMatrix();
		}
		
		void End2d() {
			glEnable(GL_DEPTH_TEST);
			glPopMatrix();
            
            _polygons = 0;
		}
		
		ENGINE_JS_METHOD(Rect) {
            ENGINE_JS_SCOPE_OPEN;

			GLfloat x, y, w, h;
            
            ENGINE_CHECK_ARGS_LENGTH(4);
            
            ENGINE_CHECK_ARG_NUMBER(0);
            ENGINE_CHECK_ARG_NUMBER(1);
            ENGINE_CHECK_ARG_NUMBER(2);
            ENGINE_CHECK_ARG_NUMBER(3);
		
			x = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(0);
			y = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(1);
			w = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(2);
			h = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(3);
            
            if (isOffscreen(x, y, w, h)) {
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            beginRendering(GL_QUADS);
				addVert(x, y, 0);
				addVert(x + w, y, 0);
				addVert(x + w, y + h, 0);
				addVert(x, y + h, 0);
            endRendering();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
		
		ENGINE_JS_METHOD(Grid) {
            ENGINE_JS_SCOPE_OPEN;
            
			GLfloat x, y, w, h;
            
            ENGINE_CHECK_ARGS_LENGTH(4);
            
            ENGINE_CHECK_ARG_NUMBER(0);
            ENGINE_CHECK_ARG_NUMBER(1);
            ENGINE_CHECK_ARG_NUMBER(2);
            ENGINE_CHECK_ARG_NUMBER(3);
            
			x = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(0);
			y = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(1);
			w = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(2);
			h = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(3);
            
            if (isOffscreen(x, y, w, h)) {
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            beginRendering(GL_LINE_LOOP);
                addVert(x, y, 0);
                addVert(x + w, y, 0);
                addVert(x + w, y + h, 0);
                addVert(x, y + h, 0);
				addVert(x, y, 0);
            endRendering();
            
			ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
		
		ENGINE_JS_METHOD(Grad) {
            ENGINE_JS_SCOPE_OPEN;
            
			GLfloat x, y, w, h;
            
            unsigned int col1, col2;
            
            bool vert;
            
            ENGINE_CHECK_ARGS_LENGTH(7);
            
            ENGINE_CHECK_ARG_NUMBER(0);
            ENGINE_CHECK_ARG_NUMBER(1);
            ENGINE_CHECK_ARG_NUMBER(2);
            ENGINE_CHECK_ARG_NUMBER(3);
            ENGINE_CHECK_ARG_INT32(4);
            ENGINE_CHECK_ARG_INT32(5);
            ENGINE_CHECK_ARG_BOOLEAN(6);
            
			x = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(0);
			y = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(1);
			w = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(2);
			h = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(3);
            
            if (isOffscreen(x, y, w, h)) {
                return scope.Close(v8::Undefined());
            }
		
			col1 = (unsigned int)ENGINE_GET_ARG_INT32_VALUE(4);
			col2 = (unsigned int)ENGINE_GET_ARG_INT32_VALUE(5);
		
			vert = ENGINE_GET_ARG_BOOLEAN_VALUE(6);
		
			if (col1 > 256 * 256 * 256) // there fixed that age old bug
			{
				// TODO : Error
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
			}
		
			if (col2 > 256 * 256 * 256)
			{
				// TODO : Error
				ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
			}
            
            float col1R = (float)((col1 & 0xff0000) >> 16) / 255;
            float col1G = (float)((col1 & 0x00ff00) >> 8) / 255;
            float col1B = (float)(col1 & 0x0000ff) / 255;
            
            float col2R = (float)((col2 & 0xff0000) >> 16) / 255;
            float col2G = (float)((col2 & 0x00ff00) >> 8) / 255;
            float col2B = (float)(col2 & 0x0000ff) / 255;
            
            beginRendering(GL_QUADS);
				if (vert) {
                    addVert(x, y, 0, col1R, col1G, col1B);
                    addVert(x + w, y, 0, col1R, col1G, col1B);
                    addVert(x, y + h, 0, col2R, col2G, col2B);
                    addVert(x + w, y + h, 0, col2R, col2G, col2B);
				} else {
                    addVert(x, y, 0, col1R, col1G, col1B);
                    addVert(x + w, y, 0, col2R, col2G, col2B);
                    addVert(x, y + h, 0, col2R, col2G, col2B);
                    addVert(x + w, y + h, 0, col1R, col1G, col1B);
				}
            endRendering();
		
			ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
		
		ENGINE_JS_METHOD(SetColorF) {
			ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(3);
            
            ENGINE_CHECK_ARG_NUMBER(0);
            ENGINE_CHECK_ARG_NUMBER(1);
            ENGINE_CHECK_ARG_NUMBER(2);
		
			_currentColorR = ENGINE_GET_ARG_NUMBER_VALUE(0);
			_currentColorG = ENGINE_GET_ARG_NUMBER_VALUE(1);
			_currentColorB = ENGINE_GET_ARG_NUMBER_VALUE(2);
		
			ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
		
		ENGINE_JS_METHOD(SetColor) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_INT32(0);
		
			int col1 = ENGINE_GET_ARG_INT32_VALUE(0);
		
			if (col1 > (256 * 256 * 256))
			{
                // TODO : Error
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
			}
		
			_currentColorR = (float)((col1 & 0xff0000) >> 16) / 255;
			_currentColorG = (float)((col1 & 0x00ff00) >> 8) / 255;
			_currentColorB = (float)(col1 & 0x0000ff) / 255;
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
		
		ENGINE_JS_METHOD(SetColorI) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(3);
            
            ENGINE_CHECK_ARG_NUMBER(0);
            ENGINE_CHECK_ARG_NUMBER(1);
            ENGINE_CHECK_ARG_NUMBER(2);
		
			_currentColorR = ENGINE_GET_ARG_NUMBER_VALUE(0) / 255;
			_currentColorG = ENGINE_GET_ARG_NUMBER_VALUE(1) / 255;
			_currentColorB = ENGINE_GET_ARG_NUMBER_VALUE(2) / 255;
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
		
		ENGINE_JS_METHOD(ClearColor) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(3);
            
            ENGINE_CHECK_ARG_NUMBER(0);
            ENGINE_CHECK_ARG_NUMBER(1);
            ENGINE_CHECK_ARG_NUMBER(2);
		
			glClearColor(ENGINE_GET_ARG_NUMBER_VALUE(0),
                         ENGINE_GET_ARG_NUMBER_VALUE(1),
                         ENGINE_GET_ARG_NUMBER_VALUE(2), 1.0f);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
		
		ENGINE_JS_METHOD(Print) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(3);
            
            ENGINE_CHECK_ARG_NUMBER(0);
            ENGINE_CHECK_ARG_NUMBER(1);
            ENGINE_CHECK_ARG_STRING(2);

            const char* str = *ENGINE_GET_ARG_CSTRING_VALUE(2);
		
			glEnable(GL_TEXTURE_2D);
            glColor3f(_currentColorR, _currentColorG, _currentColorB);
			getFont()->beginDraw(ENGINE_GET_ARG_NUMBER_VALUE(0) - _centerX,
                                 ENGINE_GET_ARG_NUMBER_VALUE(1) - _centerY) << str << getFont()->endDraw();
			glDisable(GL_TEXTURE_2D);
            
            _polygons += strlen(str) * 4;
		
			ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
        
        ENGINE_JS_METHOD(Draw) {
            ENGINE_JS_SCOPE_OPEN;
            
            CheckGLError("Pre Image Draw");
            
            GLuint texId;
			GLfloat x, y, w, h;
            
            ENGINE_CHECK_ARGS_LENGTH(5);
            
            ENGINE_CHECK_ARG_INT32(0);
            ENGINE_CHECK_ARG_NUMBER(1);
            ENGINE_CHECK_ARG_NUMBER(2);
            ENGINE_CHECK_ARG_NUMBER(3);
            ENGINE_CHECK_ARG_NUMBER(4);
            
            texId = (unsigned int)ENGINE_GET_ARG_INT32_VALUE(0);
            
			x = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(1);
			y = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(2);
			w = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(3);
			h = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(4);
            
            enableTexture(texId);
            
			beginRendering(GL_QUADS);
                //      x       y       z       r       g       b       s       t
                addVert(x,      y,      0,      1.0f,   1.0f,   1.0f,   0.0f,   1.0f);
                addVert(x + w,  y,      0,      1.0f,   1.0f,   1.0f,   1.0f,   1.0f);
                addVert(x + w,  y + h,  0,      1.0f,   1.0f,   1.0f,   1.0f,   0.0f);
                addVert(x,      y + h,  0,      1.0f,   1.0f,   1.0f,   0.0f,   0.0f);
            
			endRendering();
            
            disableTexture();
            
            CheckGLError("Post Image Draw");
            
			ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
		
		// TODO: add support for this function to open anything ( or just png\jpg\gif )
		ENGINE_JS_METHOD(OpenImage) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0);

			if (!Filesystem::FileExists(ENGINE_GET_ARG_CPPSTRING_VALUE(0))) {
				// TODO : Error
				ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
			}
		
			GLuint text = 0;
		
			glGenTextures(1, &text);
		
			glBindTexture(GL_TEXTURE_2D, text);
		
			// TODO: Write my own loader

			glfwLoadTexture2D(Filesystem::GetRealPath(ENGINE_GET_ARG_CPPSTRING_VALUE(0)).c_str(), GLFW_BUILD_MIPMAPS_BIT);
		
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
								GL_LINEAR_MIPMAP_LINEAR );
		
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
								GL_LINEAR );
            
            glBindTexture(GL_TEXTURE_2D, 0);
            
            std::cout << "Created Image from file: " << text << std::endl;
		
			ENGINE_JS_SCOPE_CLOSE(v8::Integer::New(text));
		}
        
        ENGINE_JS_METHOD(CreateImage) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(3);
            
            ENGINE_CHECK_ARG_ARRAY(0);
            ENGINE_CHECK_ARG_INT32(1);
            ENGINE_CHECK_ARG_INT32(2);
            
            v8::Local<v8::Array> arr = ENGINE_GET_ARG_ARRAY(0);
            
            int width = ENGINE_GET_ARG_INT32_VALUE(1);
            int height = ENGINE_GET_ARG_INT32_VALUE(2);
            int len = width * height;
            
            if (arr->Length() != width * height * 3) {
                std::cout << "Array size != width * height * 3 : " << arr->Length() << std::endl;
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            float pixels[len * 4];
            
            for (int i = 0; i < len * 4; i += 4) {
                pixels[i + 0] = clamp((float) arr->Get(i + 0)->NumberValue(), 0.0f, 1.0f);
                pixels[i + 1] = clamp((float) arr->Get(i + 1)->NumberValue(), 0.0f, 1.0f);
                pixels[i + 2] = clamp((float) arr->Get(i + 2)->NumberValue(), 0.0f, 1.0f);
                pixels[i + 3] = 1.0f;
            }
            
			GLuint text = 0;
            
            glEnable(GL_TEXTURE_2D);
            
			glGenTextures(1, &text);
            
			glBindTexture(GL_TEXTURE_2D, text);
            
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, &pixels);
            
            glGenerateMipmap(GL_TEXTURE_2D);
            
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                            GL_LINEAR_MIPMAP_LINEAR );
            
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                            GL_LINEAR );
            
			glBindTexture(GL_TEXTURE_2D, 0);
            
            glDisable(GL_TEXTURE_2D);
            
            std::cout << "Created Image from data: " << text << std::endl;
            
            ENGINE_JS_SCOPE_CLOSE(v8::Integer::New(text));
        }
        
        ENGINE_JS_METHOD(FreeImage) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_INT32(0);
            
            GLuint texture = ENGINE_GET_ARG_INT32_VALUE(0);
            
            glDeleteTextures(1, &texture);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        // basicly restarts 2d drawing
        ENGINE_JS_METHOD(CameraReset) {
            ENGINE_JS_SCOPE_OPEN;
            
            int oldVertCount = _polygons;
            
            End2d();
            Begin2d();
            
            _polygons = oldVertCount;
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(CameraPan) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_NUMBER(0);
            ENGINE_CHECK_ARG_NUMBER(1);
            
            glTranslatef(ENGINE_GET_ARG_NUMBER_VALUE(0), ENGINE_GET_ARG_NUMBER_VALUE(1), 0.0f);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(CameraZoom) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_NUMBER(0);
            
            double zoomFactor = args[0]->NumberValue();
            
            glScalef(zoomFactor, zoomFactor, 0.0f);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(CameraRotate) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_NUMBER(0);
            
            double rotateFactor = ENGINE_GET_ARG_NUMBER_VALUE(0);
            
            glRotatef(rotateFactor, 0.0f, 0.0f, 1.0f);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(GetTextWidth) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0);
            
            float textWidth = args[0]->ToString()->Length() * 10;
            
            ENGINE_JS_SCOPE_CLOSE(v8::Number::New(textWidth));
        }
        
        ENGINE_JS_METHOD(GetVerts) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_JS_SCOPE_CLOSE(v8::Integer::New(_polygons));
        }
        
        ENGINE_JS_METHOD(SetDrawOffscreen) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_BOOLEAN(0);
            
            _drawOffScreen = ENGINE_GET_ARG_BOOLEAN_VALUE(0);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(SetCenter) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_INT32(0);
            ENGINE_CHECK_ARG_INT32(1);
            
            _centerX = ENGINE_GET_ARG_INT32_VALUE(0);
            _centerY = ENGINE_GET_ARG_INT32_VALUE(1);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }

	} // namespace JsDraw
} // namespace Engine