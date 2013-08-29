#include "JSDraw.hpp"

namespace Engine {

	namespace JsDraw {
		
        ENGINE_JS_METHOD(LoadShader) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            ENGINE_CHECK_ARG_STRING(0, "Arg0 has to be a filename pointing to a shader");
            
            upgradeGL3();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
		ENGINE_JS_METHOD(Rect) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;

			GLfloat x, y, w, h;
            
            ENGINE_CHECK_ARGS_LENGTH(4);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 has to be X of a rect");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 has to be Y of a rect");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 has to be Width of a rect");
            ENGINE_CHECK_ARG_NUMBER(3, "Arg3 has to be Height of a rect");
		
			x = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(0);
			y = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(1);
			w = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(2);
			h = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(3);
            
            if (Draw2D::IsOffscreen(x, y, w, h)) {
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            Draw2D::Rect(x, y, w, h);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
		
		ENGINE_JS_METHOD(Grid) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
			GLfloat x, y, w, h;
            
            ENGINE_CHECK_ARGS_LENGTH(4);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 has to be X of a rect");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 has to be Y of a rect");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 has to be Width of a rect");
            ENGINE_CHECK_ARG_NUMBER(3, "Arg3 has to be Height of a rect");
            
			x = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(0);
			y = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(1);
			w = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(2);
			h = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(3);
            
            if (Draw2D::IsOffscreen(x, y, w, h)) {
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            Draw2D::Grid(x, y, w, h);
            
			ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
		
		ENGINE_JS_METHOD(Grad) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
			GLfloat x, y, w, h;
            
            unsigned int col1, col2;
            
            bool vert;
            
            ENGINE_CHECK_ARGS_LENGTH(7);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 has to be X of a rect");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 has to be Y of a rect");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 has to be Width of a rect");
            ENGINE_CHECK_ARG_NUMBER(3, "Arg3 has to be Height of a rect");
            ENGINE_CHECK_ARG_INT32(4, "Arg4 is Color1 of the gradient");
            ENGINE_CHECK_ARG_INT32(5, "Arg5 is Color2 of the gradient");
            ENGINE_CHECK_ARG_BOOLEAN(6, "Arg6 set's orientation of the gradient");
            
			x = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(0);
			y = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(1);
			w = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(2);
			h = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(3);
            
            if (Draw2D::IsOffscreen(x, y, w, h)) {
                return scope.Close(v8::Undefined());
            }
		
			col1 = (unsigned int)ENGINE_GET_ARG_INT32_VALUE(4);
			col2 = (unsigned int)ENGINE_GET_ARG_INT32_VALUE(5);
		
			vert = ENGINE_GET_ARG_BOOLEAN_VALUE(6);
            
            Draw2D::Grad(x, y, w, h, col1, col2, vert);
		
			ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
        
        ENGINE_JS_METHOD(Circle) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the X center of the circle");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 is the Y center of the circle");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 is the radius of the circle");
            
            if (args.Length() == 4) {
                ENGINE_CHECK_ARG_BOOLEAN(3, "Arg4 sets the fill style of the circle");
                Draw2D::Circle(ENGINE_GET_ARG_NUMBER_VALUE(0),
                               ENGINE_GET_ARG_NUMBER_VALUE(1),
                               ENGINE_GET_ARG_NUMBER_VALUE(2),
                               ENGINE_GET_ARG_BOOLEAN_VALUE(3));
            } else if (args.Length() == 5) {
                ENGINE_CHECK_ARG_NUMBER(3, "Arg3 is the number of sides to the circle");
                ENGINE_CHECK_ARG_BOOLEAN(4, "Arg4 sets the fill style of the circle");
                Draw2D::Circle(ENGINE_GET_ARG_NUMBER_VALUE(0),
                               ENGINE_GET_ARG_NUMBER_VALUE(1),
                               ENGINE_GET_ARG_NUMBER_VALUE(2),
                               ENGINE_GET_ARG_NUMBER_VALUE(3),
                               ENGINE_GET_ARG_BOOLEAN_VALUE(4));
            } else if (args.Length() == 7) {
                ENGINE_CHECK_ARG_NUMBER(3, "Arg3 is the number of sides to the circle");
                ENGINE_CHECK_ARG_NUMBER(4, "Arg4 is the start % of the circle");
                ENGINE_CHECK_ARG_NUMBER(5, "Arg5 is the end % of the circle");
                ENGINE_CHECK_ARG_BOOLEAN(6, "Arg6 sets the fill style of the circle");
                Draw2D::Circle(ENGINE_GET_ARG_NUMBER_VALUE(0),
                               ENGINE_GET_ARG_NUMBER_VALUE(1),
                               ENGINE_GET_ARG_NUMBER_VALUE(2),
                               ENGINE_GET_ARG_NUMBER_VALUE(3),
                               ENGINE_GET_ARG_NUMBER_VALUE(4),
                               ENGINE_GET_ARG_NUMBER_VALUE(5),
                               ENGINE_GET_ARG_BOOLEAN_VALUE(6));
            } else if (args.Length() == 8) {
                ENGINE_CHECK_ARG_NUMBER(3, "Arg3 is the inner radius of the circle");
                ENGINE_CHECK_ARG_NUMBER(4, "Arg3 is the number of sides to the circle");
                ENGINE_CHECK_ARG_NUMBER(5, "Arg4 is the start % of the circle");
                ENGINE_CHECK_ARG_NUMBER(6, "Arg5 is the end % of the circle");
                ENGINE_CHECK_ARG_BOOLEAN(7, "Arg6 sets the fill style of the circle");
                Draw2D::Circle(ENGINE_GET_ARG_NUMBER_VALUE(0),
                               ENGINE_GET_ARG_NUMBER_VALUE(1),
                               ENGINE_GET_ARG_NUMBER_VALUE(2),
                               ENGINE_GET_ARG_NUMBER_VALUE(3),
                               ENGINE_GET_ARG_NUMBER_VALUE(4),
                               ENGINE_GET_ARG_NUMBER_VALUE(5),
                               ENGINE_GET_ARG_NUMBER_VALUE(6),
                               ENGINE_GET_ARG_BOOLEAN_VALUE(7));
            } else {
                ENGINE_THROW_ARGERROR("");
            }
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(Curve) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(8);
            
            ENGINE_CHECK_ARG_NUMBER(0, "");
            ENGINE_CHECK_ARG_NUMBER(1, "");
            ENGINE_CHECK_ARG_NUMBER(2, "");
            ENGINE_CHECK_ARG_NUMBER(3, "");
            ENGINE_CHECK_ARG_NUMBER(4, "");
            ENGINE_CHECK_ARG_NUMBER(5, "");
            ENGINE_CHECK_ARG_NUMBER(6, "");
            ENGINE_CHECK_ARG_NUMBER(7, "");
            
            Draw2D::BezierCurve(ENGINE_GET_ARG_NUMBER_VALUE(0),
                                ENGINE_GET_ARG_NUMBER_VALUE(1),
                                ENGINE_GET_ARG_NUMBER_VALUE(2),
                                ENGINE_GET_ARG_NUMBER_VALUE(3),
                                ENGINE_GET_ARG_NUMBER_VALUE(4),
                                ENGINE_GET_ARG_NUMBER_VALUE(5),
                                ENGINE_GET_ARG_NUMBER_VALUE(6),
                                ENGINE_GET_ARG_NUMBER_VALUE(7));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
		
		ENGINE_JS_METHOD(SetColorF) {
			ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(3);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the Red Component between 0.0f and 1.0f");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 is the Green Component between 0.0f and 1.0f");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 is the Blue Component between 0.0f and 1.0f");
            
            Draw2D::SetColor(ENGINE_GET_ARG_NUMBER_VALUE(0),
                             ENGINE_GET_ARG_NUMBER_VALUE(1),
                             ENGINE_GET_ARG_NUMBER_VALUE(2));
		
			ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
		
		ENGINE_JS_METHOD(SetColor) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
		
            if (args[0]->IsNumber()) {
                int col = ENGINE_GET_ARG_INT32_VALUE(0);
		
                if (col > (256 * 256 * 256))
                {
                    ENGINE_THROW_ARGERROR("Arg0 is beyond 0xffffff");
                    ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
                }
		
                Draw2D::SetColor(col); // yay now draw2d handles it
            } else if (args[0]->IsString()) {
                Draw2D::SetColor(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            } else {
                ENGINE_THROW_ARGERROR("Arg0 needs to be a string(colorName) or a number(in the format 0xrrggbb)");
            }
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
		
		ENGINE_JS_METHOD(SetColorI) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(3);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the Red Component between 0 and 255");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 is the Green Component between 0 and 255");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 is the Blue Component between 0 and 255");
		
			Draw2D::SetColor(ENGINE_GET_ARG_NUMBER_VALUE(0) / 255,
                             ENGINE_GET_ARG_NUMBER_VALUE(1) / 255,
                             ENGINE_GET_ARG_NUMBER_VALUE(2) / 255);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
        
        ENGINE_JS_METHOD(GetRGBFromHSV) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(3);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the Hue Component between 0 and 360");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 is the Saturation Component between 0.0f and 1.0f");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 is the Value Component between 0.0f and 1.0f");
            
            v8::Handle<v8::Object> ret = v8::Object::New();
            
            float hue = ENGINE_GET_ARG_NUMBER_VALUE(0),
                saturation = ENGINE_GET_ARG_NUMBER_VALUE(1),
                value = ENGINE_GET_ARG_NUMBER_VALUE(2);
            
            double hh, p, q, t, ff;
            long i;
            
            if(saturation <= 0.0) {       // < is bogus, just shuts up warnings
#ifndef _PLATFORM_WIN32
                if(std::isnan(hue)) {   // in.h == NAN
#else
				if (hue != hue) {
#endif
                    ret->Set(v8::String::NewSymbol("r"), v8::Number::New(value));
                    ret->Set(v8::String::NewSymbol("g"), v8::Number::New(value));
                    ret->Set(v8::String::NewSymbol("b"), v8::Number::New(value));
                    ENGINE_JS_SCOPE_CLOSE(ret);
                }
                // error - should never happen
                ret->Set(v8::String::NewSymbol("r"), v8::Number::New(0.0f));
                ret->Set(v8::String::NewSymbol("g"), v8::Number::New(0.0f));
                ret->Set(v8::String::NewSymbol("b"), v8::Number::New(0.0f));
                ENGINE_JS_SCOPE_CLOSE(ret);
            }
            hh = hue;
            if(hh >= 360.0) hh = 0.0;
            hh /= 60.0;
            i = (long)hh;
            ff = hh - i;
            p = value * (1.0 - saturation);
            q = value * (1.0 - (saturation * ff));
            t = value * (1.0 - (saturation * (1.0 - ff)));
            
            switch(i) {
                case 0:
                    ret->Set(v8::String::NewSymbol("r"), v8::Number::New(value));
                    ret->Set(v8::String::NewSymbol("g"), v8::Number::New(t));
                    ret->Set(v8::String::NewSymbol("b"), v8::Number::New(p));
                    break;
                case 1:
                    ret->Set(v8::String::NewSymbol("r"), v8::Number::New(q));
                    ret->Set(v8::String::NewSymbol("g"), v8::Number::New(value));
                    ret->Set(v8::String::NewSymbol("b"), v8::Number::New(p));
                    break;
                case 2:
                    ret->Set(v8::String::NewSymbol("r"), v8::Number::New(p));
                    ret->Set(v8::String::NewSymbol("g"), v8::Number::New(value));
                    ret->Set(v8::String::NewSymbol("b"), v8::Number::New(t));
                    break;
                case 3:
                    ret->Set(v8::String::NewSymbol("r"), v8::Number::New(p));
                    ret->Set(v8::String::NewSymbol("g"), v8::Number::New(q));
                    ret->Set(v8::String::NewSymbol("b"), v8::Number::New(value));
                    break;
                case 4:
                    ret->Set(v8::String::NewSymbol("r"), v8::Number::New(t));
                    ret->Set(v8::String::NewSymbol("g"), v8::Number::New(p));
                    ret->Set(v8::String::NewSymbol("b"), v8::Number::New(value));
                    break;
                case 5:
                default:
                    ret->Set(v8::String::NewSymbol("r"), v8::Number::New(value));
                    ret->Set(v8::String::NewSymbol("g"), v8::Number::New(p));
                    ret->Set(v8::String::NewSymbol("b"), v8::Number::New(q));
                    break;
            }
            ENGINE_JS_SCOPE_CLOSE(ret);
        }
		
		ENGINE_JS_METHOD(ClearColor) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
		
            if (args[0]->IsNumber()) {
                int col = ENGINE_GET_ARG_INT32_VALUE(0);
                
                if (col > (256 * 256 * 256))
                {
                    ENGINE_THROW_ARGERROR("Arg0 is beyond 0xffffff");
                    ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
                }
                
                Draw2D::ClearColor(col); // yay now draw2d handles it
            } else if (args[0]->IsString()) {
                Draw2D::ClearColor(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            } else {
                ENGINE_THROW_ARGERROR("Arg0 needs to be a string(colorName) or a number(in the format 0xrrggbb)");
            }
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
        
        ENGINE_JS_METHOD(LoadFont) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the name you use to refer to the font");
            ENGINE_CHECK_ARG_STRING(1, "Arg1 is the filename of the font");
            
            Draw2D::LoadFont(ENGINE_GET_ARG_CPPSTRING_VALUE(0),
                             ENGINE_GET_ARG_CPPSTRING_VALUE(1));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(GetAllFonts) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(SetFont) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the name of the font to use");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 is the size of the font to use");
            
            Draw2D::SetFont(ENGINE_GET_ARG_CPPSTRING_VALUE(0),
                            ENGINE_GET_ARG_NUMBER_VALUE(1));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(IsFontLoaded) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the name of the font to check");
            
            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(Draw2D::IsFontLoaded(ENGINE_GET_ARG_CPPSTRING_VALUE(0))));
        }
		
		ENGINE_JS_METHOD(Print) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(3);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the X position of Arg2");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 is the Y position of Arg2");
            ENGINE_CHECK_ARG_STRING(2, "Arg2 is the string to print");

            std::string str = ENGINE_GET_ARG_CPPSTRING_VALUE(2);

			Draw2D::Print(ENGINE_GET_ARG_NUMBER_VALUE(0), ENGINE_GET_ARG_NUMBER_VALUE(1), (const char*) str.c_str());
		
			ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
        
        ENGINE_JS_METHOD(Draw) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            Draw2D::CheckGLError("Pre Image Draw");
            
            GLuint texId;
			GLfloat x, y, w, h;
            
            ENGINE_CHECK_ARGS_LENGTH(5);
            
            ENGINE_CHECK_ARG_INT32(0, "Arg0 is a valid textureID that's been loaded since the last context change");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 has to be X of a rect");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 has to be Y of a rect");
            ENGINE_CHECK_ARG_NUMBER(3, "Arg3 has to be Width of a rect");
            ENGINE_CHECK_ARG_NUMBER(4, "Arg4 has to be Height of a rect");
            
            texId = (unsigned int)ENGINE_GET_ARG_INT32_VALUE(0);
            
            if (!Draw2D::IsValidTextureID(texId)) {
                ENGINE_THROW_ARGERROR("Arg0 is not a valid textureID");
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
			x = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(1);
			y = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(2);
			w = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(3);
			h = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(4);
            
            Draw2D::EnableTexture(texId);
            
            Draw2D::BeginRendering(GL_QUADS);
            //              x         y         z   s     t
            Draw2D::AddVert(x,        y,        0,  0.0f, 0.0f);
            Draw2D::AddVert(x + w,    y,        0,  1.0f, 0.0f);
            Draw2D::AddVert(x + w,    y + h,    0,  1.0f, 1.0f);
            Draw2D::AddVert(x,        y + h,    0,  0.0f, 1.0f);
            
            Draw2D::EndRendering();
            
            Draw2D::DisableTexture();
            
            Draw2D::CheckGLError("Post Image Draw");
            
			ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
        
        ENGINE_JS_METHOD(DrawSub) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            Draw2D::CheckGLError("Pre Image Draw");
            
            GLuint texId;
			GLfloat x1, y1, w1, h1,
                    x2, y2, w2, h2;
            int imageWidth, imageHeight;
            
            ENGINE_CHECK_ARGS_LENGTH(9);
            
            ENGINE_CHECK_ARG_INT32(0, "Arg0 is a valid textureID that's been loaded since the last context change");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 has to be X of a rect");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 has to be Y of a rect");
            ENGINE_CHECK_ARG_NUMBER(3, "Arg3 has to be Width of a rect");
            ENGINE_CHECK_ARG_NUMBER(4, "Arg4 has to be Height of a rect");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg5 has to be X of a sub rectangle");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg6 has to be Y of a sub rectangle");
            ENGINE_CHECK_ARG_NUMBER(3, "Arg7 has to be Width of a sub rectangle");
            ENGINE_CHECK_ARG_NUMBER(4, "Arg8 has to be Height of a sub rectangle");
            
            texId = (unsigned int)ENGINE_GET_ARG_INT32_VALUE(0);
            
            if (!Draw2D::IsValidTextureID(texId)) {
                ENGINE_THROW_ARGERROR("Arg0 is not a valid textureID");
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
			x1 = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(1);
			y1 = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(2);
			w1 = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(3);
			h1 = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(4);
			x2 = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(5);
			y2 = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(6);
			w2 = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(7);
			h2 = (GLfloat)ENGINE_GET_ARG_NUMBER_VALUE(8);
            
            Draw2D::EnableTexture(texId);
            
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &imageWidth);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &imageHeight);
            
			Draw2D::BeginRendering(GL_QUADS);
            //      x           y           z       s                       t
            Draw2D::AddVert(x1,         y1,         0,      x2 / imageWidth,        y2 / imageHeight);
            Draw2D::AddVert(x1 + w1,    y1,         0,      (x2 + w2) / imageWidth, y2 / imageHeight);
            Draw2D::AddVert(x1 + w1,    y1 + h1,    0,      (x2 + w2) / imageWidth, (y2 + h2) / imageHeight);
            Draw2D::AddVert(x1,         y1 + h1,    0,      x2 / imageWidth,        (y2 + h2) / imageHeight);
            
			Draw2D::EndRendering();
            
            Draw2D::DisableTexture();
            
            Draw2D::CheckGLError("Post Image Draw");
            
			ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
		
		ENGINE_JS_METHOD(OpenImage) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the filename of the image to load");
            
            Draw2D::CheckGLError("Pre Image Load");

			if (!Filesystem::FileExists(ENGINE_GET_ARG_CPPSTRING_VALUE(0))) {
                ENGINE_THROW_ARGERROR("File does not Exist");
				ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
			}
            
            std::string filename = ENGINE_GET_ARG_CPPSTRING_VALUE(0);
            
            if (!ResourceManager::HasSource(filename)) {
                ResourceManager::Load(filename);
            }
            
            ResourceManager::ImageResource* img = new ResourceManager::ImageResource(filename);
            img->Load();
		
			ENGINE_JS_SCOPE_CLOSE(v8::Integer::New(img->GetTextureID()));
		}
        
        ENGINE_JS_METHOD(CreateImageArray) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_INT32(0, "Arg0 is the width of the new Image Array");
            ENGINE_CHECK_ARG_INT32(1, "Arg1 is the height of the new Image Array");
            
            int arraySize = ENGINE_GET_ARG_INT32_VALUE(0) * ENGINE_GET_ARG_INT32_VALUE(1) * 4;
            
            v8::Handle<v8::Object> array = v8::Object::New();
            
            v8::Isolate* isolate = v8::Isolate::GetCurrent();
            
            float* rawArray = new float[arraySize]; // sadly this is a memory leak right now
            
            for (int i = 0; i < arraySize; i += 4) {
                rawArray[i] = 1.0f;
                rawArray[i + 1] = 1.0f;
                rawArray[i + 2] = 1.0f;
                rawArray[i + 3] = 1.0f;
            }
            
            v8::Persistent<v8::Object> persistent_array = v8::Persistent<v8::Object>::New(isolate, array);
            persistent_array.MarkIndependent(isolate); // and this is also a bad memory leak
            isolate->AdjustAmountOfExternalAllocatedMemory(arraySize * sizeof(float));
            
            array->SetIndexedPropertiesToExternalArrayData(rawArray, v8::kExternalFloatArray, arraySize * sizeof(float));
            
            ENGINE_JS_SCOPE_CLOSE(array);
        }
        
        ENGINE_JS_METHOD(CreateImage) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(3);
            
            ENGINE_CHECK_ARG_OBJECT(0, "Arg0 is a Object");
            ENGINE_CHECK_ARG_INT32(1, "Arg1 is the width of the new image");
            ENGINE_CHECK_ARG_INT32(2, "Arg2 is the height of the new image");
            
            v8::Local<v8::Array> arr = ENGINE_GET_ARG_ARRAY(0);
            
            int width = ENGINE_GET_ARG_INT32_VALUE(1);
            int height = ENGINE_GET_ARG_INT32_VALUE(2);
            
            float* pixels = NULL;
            
            if (arr->HasIndexedPropertiesInExternalArrayData()) {
                pixels = (float*) arr->GetIndexedPropertiesExternalArrayData();
            } else {
                int len = width * height;
                
                bool noClamp = !Config::GetBoolean("draw_clampCreateTexture");
                
                if (arr->Length() != width * height * 3) {
                    ENGINE_THROW_ARGERROR("Array size != width * height * 3");
                    ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
                }
                
                pixels = (float*) malloc(sizeof(float) * len * 4);
                
                int i2 = 0;
                
                for (int i = 0; i < len * 4; i += 4) {
                    if (noClamp) {
                        pixels[i + 0] = (float) arr->Get(i2 + 0)->NumberValue();
                        pixels[i + 1] = (float) arr->Get(i2 + 1)->NumberValue();
                        pixels[i + 2] = (float) arr->Get(i2 + 2)->NumberValue();
                        pixels[i + 3] = 1.0f;
                    } else {
                        pixels[i + 0] = glm::clamp((float) arr->Get(i2 + 0)->NumberValue(), 0.0f, 1.0f);
                        pixels[i + 1] = glm::clamp((float) arr->Get(i2 + 1)->NumberValue(), 0.0f, 1.0f);
                        pixels[i + 2] = glm::clamp((float) arr->Get(i2 + 2)->NumberValue(), 0.0f, 1.0f);
                        pixels[i + 3] = 1.0f;
                    }
                    i2 += 3;
                }
            }
            
			GLuint text = 0;
            
            glEnable(GL_TEXTURE_2D);
            
			glGenTextures(1, &text);
            
			glBindTexture(GL_TEXTURE_2D, text);
            
            if (Config::GetBoolean("draw_createImageMipmap")) {
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                            GL_NEAREST_MIPMAP_NEAREST );
            } else {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            }
            
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                            GL_NEAREST );
            
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, pixels);
            
            if (Config::GetBoolean("draw_createImageMipmap")) {
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            
			glBindTexture(GL_TEXTURE_2D, 0);
            
            glDisable(GL_TEXTURE_2D);
            
            if (!arr->HasIndexedPropertiesInExternalArrayData()) {
                free(pixels);
            }
            
            if (Config::GetBoolean("log_createImage")) {
                Logger::begin("JSDraw", Logger::LogLevel_Log) << "Created Image from data: " << text << Logger::end();
            }
            
            ENGINE_JS_SCOPE_CLOSE(v8::Integer::New(text));
        }
        
        ENGINE_JS_METHOD(FreeImage) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_INT32(0, "Arg0 is the textureID of the texture to free");
            
            GLuint texture = ENGINE_GET_ARG_INT32_VALUE(0);
            
            if (!Draw2D::IsValidTextureID(texture)) {
                ENGINE_THROW_ARGERROR("Arg0 is not a valid textureID");
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            glDeleteTextures(1, &texture);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(IsTexture) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_INT32(0, "Arg0 is the textureID of the texture to check");
            
            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(Draw2D::IsValidTextureID(ENGINE_GET_ARG_INT32_VALUE(0))));
        }
        
        // basicly restarts 2d drawing
        ENGINE_JS_METHOD(CameraReset) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            Draw2D::Reset();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(CameraPan) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the X Distince to pan");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 is the Y Distince to pan");
            
            glTranslatef(ENGINE_GET_ARG_NUMBER_VALUE(0), ENGINE_GET_ARG_NUMBER_VALUE(1), 0.0f);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(CameraZoom) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the factor to zoom the camera");
            
            double zoomFactor = args[0]->NumberValue();
            
            glScalef(zoomFactor, zoomFactor, 0.0f);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(CameraRotate) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the factor to rotate the camera");
            
            double rotateFactor = ENGINE_GET_ARG_NUMBER_VALUE(0);
            
            glRotatef(rotateFactor, 0.0f, 0.0f, 1.0f);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(GetTextWidth) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the string to return the width of");
            
            ENGINE_JS_SCOPE_CLOSE(v8::Number::New(Draw2D::CalcStringWidth(ENGINE_GET_ARG_CPPSTRING_VALUE(0))));
        }
        
        ENGINE_JS_METHOD(GetVerts) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_JS_SCOPE_CLOSE(v8::Integer::New(Draw2D::GetVerts()));
        }
        
        ENGINE_JS_METHOD(SetDrawOffscreen) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_BOOLEAN(0, "Set Arg0 to false to disable drawing offscreen");
            
            Draw2D::SetDrawOffscreen(ENGINE_GET_ARG_BOOLEAN_VALUE(0));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(SetCenter) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_INT32(0, "Arg0 is the x to offset drawing by");
            ENGINE_CHECK_ARG_INT32(1, "Arg1 is the y to offset drawing by");
            
            Draw2D::SetCenter(ENGINE_GET_ARG_INT32_VALUE(0),
                              ENGINE_GET_ARG_INT32_VALUE(1));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
#define addItem(table, js_name, funct) table->Set(js_name, v8::FunctionTemplate::New(funct))
        
        void InitDraw(v8::Handle<v8::ObjectTemplate> drawTable) {
            addItem(drawTable, "loadShader", LoadShader);
            
            addItem(drawTable, "rect", Rect);
            addItem(drawTable, "grid", Grid);
            addItem(drawTable, "grad", Grad);
            addItem(drawTable, "circle", Circle);
            addItem(drawTable, "curve", Curve);
            addItem(drawTable, "setColorF", SetColorF);
            addItem(drawTable, "setColor", SetColor);
            addItem(drawTable, "setColorI", SetColorI);
            addItem(drawTable, "clearColor", ClearColor);
            addItem(drawTable, "getRGBFromHSV", GetRGBFromHSV);
            addItem(drawTable, "print", Print);
            
            addItem(drawTable, "draw", Draw);
            addItem(drawTable, "drawSub", DrawSub);
            addItem(drawTable, "openImage", OpenImage);
            addItem(drawTable, "createImageArray", CreateImageArray);
            addItem(drawTable, "createImage", CreateImage);
            addItem(drawTable, "freeImage", FreeImage);
            addItem(drawTable, "isTexture", IsTexture);
            
            addItem(drawTable, "cameraReset", CameraReset);
            addItem(drawTable, "cameraPan", CameraPan);
            addItem(drawTable, "cameraZoom", CameraZoom);
            addItem(drawTable, "cameraRotate", CameraRotate);
            
            addItem(drawTable, "getTextWidth", GetTextWidth);
            addItem(drawTable, "setFont", SetFont);
            addItem(drawTable, "loadFont", LoadFont);
            addItem(drawTable, "isFontLoaded", IsFontLoaded);
            
            addItem(drawTable, "getVerts", GetVerts);
            addItem(drawTable, "setDrawOffscreen", SetDrawOffscreen);
            addItem(drawTable, "setCenter", SetCenter);
        }
        
#undef addItem

	} // namespace JsDraw
} // namespace Engine