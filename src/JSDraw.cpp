/*
   Filename: JSDraw.cpp
   Purpose:  Javascript bindings for Engine::Draw2D
 
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

#include "JSDraw.hpp"

#include "vendor/soil/SOIL.h"

#include "Filesystem.hpp"
#include "Draw2D.hpp"

#include "ResourceManager.hpp"

#include "Application.hpp"

#include "Scripting.hpp"
#include "Util.hpp"
#include "Config.hpp"
#include "FontSheet.hpp"
#include "GL3Buffer.hpp"

namespace Engine {
    
	namespace JsDraw {
        
        static void ReadBufferWeakCallback(const v8::WeakCallbackData<v8::Object, float>& args) {
            
            size_t byte_length =
            args.GetValue()->Get(v8::String::NewFromUtf8(args.GetIsolate(), "byteLength"))->Int32Value();
            
            args.GetIsolate()->AdjustAmountOfExternalAllocatedMemory(-static_cast<intptr_t>(byte_length));
            
            delete[] args.GetParameter();
        }
        
        ENGINE_JS_METHOD(CreateColor) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Isolate* isolate = args.GetIsolate();
            
            if (!args.IsConstructCall()) {
                const int argc = args.Length();
                std::vector<v8::Handle<v8::Value>> av(static_cast<size_t>(argc),v8::Undefined(isolate));
                for(int i = 0; i < argc; ++i) av[i] = args[i];
                ENGINE_JS_SCOPE_CLOSE(args.Callee()->NewInstance(argc, &av[0]));
            }
            
            Color4f col;
            
            if (args.Length() == 0) { // new Color()
                col = Color4f();
            } else if (args.Length() == 1) { // new Color(number(0xrrggbb) || string('predefined'))
                if (args[0]->IsNumber()) {
                    col = Color4f(ENGINE_GET_ARG_INT32_VALUE(0));
                } else if (args[0]->IsString()) {
                    col = Color4f(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
                } else {
                    col = Color4f();
                }
            } else if (args.Length() == 2) { // new Color(number(0xrrggbb), number(alpha))
                col = Color4f(ENGINE_GET_ARG_INT32_VALUE(0));
                col.a = ENGINE_GET_ARG_NUMBER_VALUE(1);
            } else if (args.Length() == 3) { // new Color(number(r), number(g), number(b))
                col = Color4f(ENGINE_GET_ARG_NUMBER_VALUE(0),
                              ENGINE_GET_ARG_NUMBER_VALUE(1),
                              ENGINE_GET_ARG_NUMBER_VALUE(2),
                              1.0f);
            } else if (args.Length() == 4) { // new Color(number(r), number(g), number(b), number(a))
                col = Color4f(ENGINE_GET_ARG_NUMBER_VALUE(0),
                              ENGINE_GET_ARG_NUMBER_VALUE(1),
                              ENGINE_GET_ARG_NUMBER_VALUE(2),
                              1.0f);
            } else { // undefined
                col = Color4f();
            }
            
            args.This()->Set(v8::String::NewFromUtf8(isolate, "r"),
                             v8::Number::New(isolate, col.r));
            args.This()->Set(v8::String::NewFromUtf8(isolate, "g"),
                             v8::Number::New(isolate, col.g));
            args.This()->Set(v8::String::NewFromUtf8(isolate, "b"),
                             v8::Number::New(isolate, col.b));
            args.This()->Set(v8::String::NewFromUtf8(isolate, "a"),
                             v8::Number::New(isolate, col.a));
            
            return; // Override the normal behavior of ENGINE_JS_SCOPE_CLOSE;
        }
        
        ENGINE_JS_METHOD(Color_FromHSV) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Isolate* isolate = args.GetIsolate();
            
            ENGINE_CHECK_ARGS_LENGTH(3);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the Hue Component between 0 and 360");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 is the Saturation Component between 0.0f and 1.0f");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 is the Value Component between 0.0f and 1.0f");
            
            v8::Handle<v8::Object> ret = v8::Object::New(isolate);
            
            float hue = ENGINE_GET_ARG_NUMBER_VALUE(0),
            saturation = ENGINE_GET_ARG_NUMBER_VALUE(1),
            value = ENGINE_GET_ARG_NUMBER_VALUE(2);
            
            Color4f col = Color4f::FromHSV(hue, saturation, value);
            
            std::vector<v8::Handle<v8::Value>> av(4, v8::Undefined(isolate));
            
            av[0] = v8::Number::New(isolate, col.r);
            av[1] = v8::Number::New(isolate, col.g);
            av[2] = v8::Number::New(isolate, col.b);
            av[3] = v8::Number::New(isolate, col.a);
            
            ENGINE_JS_SCOPE_CLOSE(args.This().As<v8::Function>()->NewInstance(4, &av[0]));
        }
        
        ENGINE_JS_METHOD(Color_FromRandom) {
            ENGINE_JS_SCOPE_OPEN;
            
            static BasicRandom rand = BasicRandom();
            
            v8::Isolate* isolate = args.GetIsolate();
            
            std::vector<v8::Handle<v8::Value>> av(4, v8::Undefined(isolate));
            
            av[0] = v8::Number::New(isolate, rand.NextDouble());
            av[1] = v8::Number::New(isolate, rand.NextDouble());
            av[2] = v8::Number::New(isolate, rand.NextDouble());
            av[3] = v8::Number::New(isolate, 1.0f);
            
            ENGINE_JS_SCOPE_CLOSE(args.This().As<v8::Function>()->NewInstance(4, &av[0]));
        }
        
        ENGINE_JS_METHOD(Color_ToString) {
            ENGINE_JS_SCOPE_OPEN;
            
            std::stringstream sb;
            
            sb << "[color rgba(";
            
            sb << std::floor(args.This()->Get(v8::String::NewFromUtf8(args.GetIsolate(), "r"))->NumberValue() * 255) << ", ";
            sb << std::floor(args.This()->Get(v8::String::NewFromUtf8(args.GetIsolate(), "g"))->NumberValue() * 255) << ", ";
            sb << std::floor(args.This()->Get(v8::String::NewFromUtf8(args.GetIsolate(), "b"))->NumberValue() * 255) << ", ";
            sb << std::floor(args.This()->Get(v8::String::NewFromUtf8(args.GetIsolate(), "a"))->NumberValue() * 255);
            
            sb << ") ]";
            
            ENGINE_JS_SCOPE_CLOSE(v8::String::NewFromUtf8(args.GetIsolate(), sb.str().c_str()));
        }
        
        void InitColorObject(v8::Handle<v8::ObjectTemplate> drawTable) {
            v8::Isolate* isolate = v8::Isolate::GetCurrent();
            v8::HandleScope scope(isolate);
            
            v8::Handle<v8::FunctionTemplate> newColor = v8::FunctionTemplate::New(isolate);
            
            newColor->SetClassName(v8::String::NewFromUtf8(isolate, "Color"));
            
            v8::Handle<v8::ObjectTemplate> colorPrototypeTemplate = newColor->PrototypeTemplate();
            v8::Handle<v8::ObjectTemplate> colorInstanceTemplate = newColor->InstanceTemplate();
            
            newColor->Set(isolate, "fromHSV", v8::FunctionTemplate::New(isolate, Color_FromHSV));
            newColor->Set(isolate, "fromRandom", v8::FunctionTemplate::New(isolate,
                Color_FromRandom));
            
            colorPrototypeTemplate->Set(isolate, "toString", v8::FunctionTemplate::New(isolate, Color_ToString));
            
            newColor->SetCallHandler(CreateColor);
            
            drawTable->Set(isolate, "Color", newColor);
        }
        
        ENGINE_JS_METHOD(CreateVertexBuffer) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Isolate* isolate = args.GetIsolate();
            
            if (!args.IsConstructCall()) {
                const int argc = args.Length();
                std::vector<v8::Handle<v8::Value>> av(static_cast<size_t>(argc),v8::Undefined(isolate));
                for(int i = 0; i < argc; ++i) av[i] = args[i];
                ENGINE_JS_SCOPE_CLOSE(args.Callee()->NewInstance(argc, &av[0]));
            }
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the Effect filename to use");
            
            RenderDriverPtr render = GetAppSingilton()->GetRender();
            
            GL3BufferPtr buf = NULL;
            
            if (render->GetRendererType() == RendererType::OpenGL3) {
                EffectParametersPtr effect = EffectReader::GetEffectFromFile(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
                effect->CreateShader();
                buf = new GL3Buffer(render, effect);
            }
            
            args.This()->SetHiddenValue(v8::String::NewFromUtf8(isolate, "_buf"), v8::External::New(isolate, buf));
            
            return;
        }
        
        ENGINE_JS_METHOD(VertexBuffer_AddVert) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Isolate* isolate = args.GetIsolate();
            
            glm::vec3 pos;
            Color4f col = Color4f("white");
            glm::vec2 uv;
            
            if (args.Length() >= 2) { // (x, y)
                pos = glm::vec3(ENGINE_GET_ARG_NUMBER_VALUE(0),
                                ENGINE_GET_ARG_NUMBER_VALUE(1),
                                0.0f);
            }
            if (args.Length() >= 3) { // (x, y, col)
                v8::Handle<v8::Object> obj = ENGINE_GET_ARG_OBJECT(2);
                col.r = obj->Get(v8::String::NewFromUtf8(isolate, "r"))->NumberValue();
                col.g = obj->Get(v8::String::NewFromUtf8(isolate, "g"))->NumberValue();
                col.b = obj->Get(v8::String::NewFromUtf8(isolate, "b"))->NumberValue();
                col.a = obj->Get(v8::String::NewFromUtf8(isolate, "a"))->NumberValue();
            }
            if (args.Length() >= 5) { // (x, y, col, u, v)
                uv = glm::vec2(ENGINE_GET_ARG_NUMBER_VALUE(3),
                               ENGINE_GET_ARG_NUMBER_VALUE(4));
            }
            
            void* v = args.This()->GetHiddenValue(v8::String::NewFromUtf8(isolate, "_buf")).As<v8::External>()->Value();
            
            if (v == NULL) {
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            GL3BufferPtr buf = (GL3BufferPtr) v;
            
            buf->AddVert(pos, col, uv);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(VertexBuffer_Draw) {
            ENGINE_JS_SCOPE_OPEN;
            
            void* v = args.This()->GetHiddenValue(v8::String::NewFromUtf8(args.GetIsolate(), "_buf")).As<v8::External>()->Value();
            
            if (v == NULL) {
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            GL3BufferPtr buf = (GL3BufferPtr) v;
            
            buf->Draw(PolygonMode::Triangles, glm::mat4(), glm::mat4());
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        void InitVertexBuffer2DObject(v8::Handle<v8::ObjectTemplate> drawTable) {
            v8::Isolate* isolate = v8::Isolate::GetCurrent();
            v8::HandleScope scope(isolate);
            
            v8::Handle<v8::FunctionTemplate> newVertexBuffer = v8::FunctionTemplate::New(isolate);
            
            newVertexBuffer->SetClassName(v8::String::NewFromUtf8(isolate, "VertexBuffer2D"));
            
            v8::Handle<v8::ObjectTemplate> vbPrototypeTemplate = newVertexBuffer->PrototypeTemplate();
            v8::Handle<v8::ObjectTemplate> vbInstanceTemplate = newVertexBuffer->InstanceTemplate();
            
            vbPrototypeTemplate->Set(isolate, "addVert", v8::FunctionTemplate::New(isolate, VertexBuffer_AddVert));
            vbPrototypeTemplate->Set(isolate, "draw", v8::FunctionTemplate::New(isolate, VertexBuffer_Draw));
            
            newVertexBuffer->SetCallHandler(CreateVertexBuffer);
            
            drawTable->Set(isolate, "VertexBuffer2D", newVertexBuffer);
        }
        
        inline Draw2DPtr GetDraw2D(v8::Local<v8::Object> thisValue) {
            return (Draw2DPtr) thisValue->GetHiddenValue(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "_draw")).As<v8::External>()->Value();
        }
        
		ENGINE_JS_METHOD(Rect) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
			float x, y, w, h;
            
            ENGINE_CHECK_ARGS_LENGTH(4);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 has to be X of a rect");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 has to be Y of a rect");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 has to be Width of a rect");
            ENGINE_CHECK_ARG_NUMBER(3, "Arg3 has to be Height of a rect");
            
			x = (float)ENGINE_GET_ARG_NUMBER_VALUE(0);
			y = (float)ENGINE_GET_ARG_NUMBER_VALUE(1);
			w = (float)ENGINE_GET_ARG_NUMBER_VALUE(2);
			h = (float)ENGINE_GET_ARG_NUMBER_VALUE(3);
            
            GetDraw2D(args.This())->Rect(x, y, w, h);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
		
		ENGINE_JS_METHOD(Grid) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
			float x, y, w, h;
            
            ENGINE_CHECK_ARGS_LENGTH(4);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 has to be X of a rect");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 has to be Y of a rect");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 has to be Width of a rect");
            ENGINE_CHECK_ARG_NUMBER(3, "Arg3 has to be Height of a rect");
            
			x = (float)ENGINE_GET_ARG_NUMBER_VALUE(0);
			y = (float)ENGINE_GET_ARG_NUMBER_VALUE(1);
			w = (float)ENGINE_GET_ARG_NUMBER_VALUE(2);
			h = (float)ENGINE_GET_ARG_NUMBER_VALUE(3);
            
            GetDraw2D(args.This())->Grid(x, y, w, h);
            
			ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
		
		ENGINE_JS_METHOD(Grad) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
			float x, y, w, h;
            
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
            
			x = (float)ENGINE_GET_ARG_NUMBER_VALUE(0);
			y = (float)ENGINE_GET_ARG_NUMBER_VALUE(1);
			w = (float)ENGINE_GET_ARG_NUMBER_VALUE(2);
			h = (float)ENGINE_GET_ARG_NUMBER_VALUE(3);
            
			col1 = (unsigned int)ENGINE_GET_ARG_INT32_VALUE(4);
			col2 = (unsigned int)ENGINE_GET_ARG_INT32_VALUE(5);
            
			vert = ENGINE_GET_ARG_BOOLEAN_VALUE(6);
            
            GetDraw2D(args.This())->Grad(x, y, w, h, col1, col2, vert);
            
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
                GetDraw2D(args.This())->Circle(ENGINE_GET_ARG_NUMBER_VALUE(0),
                                               ENGINE_GET_ARG_NUMBER_VALUE(1),
                                               ENGINE_GET_ARG_NUMBER_VALUE(2),
                                               ENGINE_GET_ARG_BOOLEAN_VALUE(3));
            } else if (args.Length() == 5) {
                ENGINE_CHECK_ARG_NUMBER(3, "Arg3 is the number of sides to the circle");
                ENGINE_CHECK_ARG_BOOLEAN(4, "Arg4 sets the fill style of the circle");
                GetDraw2D(args.This())->Circle(ENGINE_GET_ARG_NUMBER_VALUE(0),
                                               ENGINE_GET_ARG_NUMBER_VALUE(1),
                                               ENGINE_GET_ARG_NUMBER_VALUE(2),
                                               ENGINE_GET_ARG_NUMBER_VALUE(3),
                                               ENGINE_GET_ARG_BOOLEAN_VALUE(4));
            } else if (args.Length() == 7) {
                ENGINE_CHECK_ARG_NUMBER(3, "Arg3 is the number of sides to the circle");
                ENGINE_CHECK_ARG_NUMBER(4, "Arg4 is the start % of the circle");
                ENGINE_CHECK_ARG_NUMBER(5, "Arg5 is the end % of the circle");
                ENGINE_CHECK_ARG_BOOLEAN(6, "Arg6 sets the fill style of the circle");
                GetDraw2D(args.This())->Circle(ENGINE_GET_ARG_NUMBER_VALUE(0),
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
                GetDraw2D(args.This())->Circle(ENGINE_GET_ARG_NUMBER_VALUE(0),
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
        
        ENGINE_JS_METHOD(Line) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(4);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the First X point of the line");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 is the First Y point of the line");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 is the Second X point of the line");
            ENGINE_CHECK_ARG_NUMBER(3, "Arg3 is the Second Y point of the line");
            
            GetDraw2D(args.This())->Line(ENGINE_GET_ARG_NUMBER_VALUE(0), ENGINE_GET_ARG_NUMBER_VALUE(1),
                                         ENGINE_GET_ARG_NUMBER_VALUE(2), ENGINE_GET_ARG_NUMBER_VALUE(3));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(Polygon) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(3);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the X center of the polygon");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 is the Y center of the polygon");
            ENGINE_CHECK_ARG_ARRAY(2, "Arg2 is the points to the circle in the format [x, y, x, y]");
            
            v8::Handle<v8::Array> arr = args[2].As<v8::Array>();
            
            float* circlePoints = new float[arr->Length()];
            int circlePointCount = arr->Length() / 2;
            
            for (int i = 0; i < arr->Length(); i++) {
                circlePoints[i] = arr->Get(i).As<v8::Number>()->Value();
            }
            
            GetDraw2D(args.This())->Polygon(ENGINE_GET_ARG_NUMBER_VALUE(0), ENGINE_GET_ARG_NUMBER_VALUE(1), circlePoints, circlePointCount);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(Curve) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(8);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the x value of point 1");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 is the y value of point 1");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 is the x value of point 2");
            ENGINE_CHECK_ARG_NUMBER(3, "Arg3 is the y value of point 2");
            ENGINE_CHECK_ARG_NUMBER(4, "Arg4 is the x value of point 3");
            ENGINE_CHECK_ARG_NUMBER(5, "Arg5 is the y value of point 3");
            ENGINE_CHECK_ARG_NUMBER(6, "Arg6 is the x value of point 4");
            ENGINE_CHECK_ARG_NUMBER(7, "Arg7 is the y value of point 4");
            
            GetDraw2D(args.This())->BezierCurve(ENGINE_GET_ARG_NUMBER_VALUE(0),
                                                ENGINE_GET_ARG_NUMBER_VALUE(1),
                                                ENGINE_GET_ARG_NUMBER_VALUE(2),
                                                ENGINE_GET_ARG_NUMBER_VALUE(3),
                                                ENGINE_GET_ARG_NUMBER_VALUE(4),
                                                ENGINE_GET_ARG_NUMBER_VALUE(5),
                                                ENGINE_GET_ARG_NUMBER_VALUE(6),
                                                ENGINE_GET_ARG_NUMBER_VALUE(7));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
		
        ENGINE_JS_METHOD(ColorPalette) {
            ENGINE_JS_SCOPE_OPEN;
            
            if (args.Length() == 2) {
                ENGINE_CHECK_ARG_STRING(0, "Arg0 is a name for the color");
                ENGINE_CHECK_ARG_INT32(1, "Arg1 is the color to associate the name with");
                
                Color4f::SetDefinedColor(ENGINE_GET_ARG_CPPSTRING_VALUE(0), ENGINE_GET_ARG_INT32_VALUE(1));
            } else if (args.Length() == 1) {
                ENGINE_CHECK_ARG_OBJECT(0, "Arg0 is an object containing a list of colors");
                
                v8::Local<v8::Object> obj = ENGINE_GET_ARG_OBJECT(0);
                
                v8::Local<v8::Array> objNames = obj->GetPropertyNames();
                
                for (int i = 0; i < objNames->Length(); i++) {
                    v8::Local<v8::String> objKey = objNames->Get(i)->ToString();
                    v8::Local<v8::Value> objItem = obj->Get(objKey);
                    
                    Color4f::SetDefinedColor(std::string(*v8::String::Utf8Value(objKey)), (int) objItem->NumberValue());
                }
            } else {
                ENGINE_THROW_ARGERROR("draw.colorPalette takes 1 or 2 args");
            }
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
		ENGINE_JS_METHOD(SetColorF) {
			ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(3);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the Red Component between 0.0f and 1.0f");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 is the Green Component between 0.0f and 1.0f");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 is the Blue Component between 0.0f and 1.0f");
            
            GetDraw2D(args.This())->GetRender()->SetColor(
                ENGINE_GET_ARG_NUMBER_VALUE(0), ENGINE_GET_ARG_NUMBER_VALUE(1), ENGINE_GET_ARG_NUMBER_VALUE(2));
            
			ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
        
        bool _setColor(v8::Isolate* isolate, Draw2DPtr draw2D, v8::Local<v8::Value> value) {
            if (value->IsNumber()) {
                int col = value.As<v8::Number>()->NumberValue();
                
                if (col > (256 * 256 * 256)) {
                    return false;
                }
                
                draw2D->GetRender()->SetColor(col); // yay now draw2d handles it
                return true;
            } else if (value->IsString()) {
                draw2D->GetRender()->SetColor(std::string(*v8::String::Utf8Value(value)));
                return true;
            } else if (value->IsObject()) {
                v8::Local<v8::Object> obj = value.As<v8::Object>();
                double r = obj->Get(v8::String::NewFromUtf8(isolate, "r"))->NumberValue();
                double g = obj->Get(v8::String::NewFromUtf8(isolate, "g"))->NumberValue();
                double b = obj->Get(v8::String::NewFromUtf8(isolate, "b"))->NumberValue();
                double a = obj->Get(v8::String::NewFromUtf8(isolate, "a"))->NumberValue();
                draw2D->GetRender()->SetColor(r, g, b, a);
                return true;
            } else {
                return false;
            }
        }
		
		ENGINE_JS_METHOD(SetColor) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            _setColor(args.GetIsolate(), GetDraw2D(args.This()), args[0]);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
		
		ENGINE_JS_METHOD(SetColorI) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(3);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the Red Component between 0 and 255");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 is the Green Component between 0 and 255");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 is the Blue Component between 0 and 255");
            
			GetDraw2D(args.This())->GetRender()->SetColor(
                ENGINE_GET_ARG_NUMBER_VALUE(0) / 255, ENGINE_GET_ARG_NUMBER_VALUE(1) / 255, ENGINE_GET_ARG_NUMBER_VALUE(2) / 255);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
        
        v8::Local<v8::Value> _getColor(v8::Isolate* isolate, Draw2DPtr draw) {
            v8::Local<v8::Object> ret = v8::Object::New(isolate);
            
            Color4f col = draw->GetRender()->GetColor();
            
            ret->Set(v8::String::NewFromUtf8(isolate, "r"), v8::Number::New(isolate, col.r));
            ret->Set(v8::String::NewFromUtf8(isolate, "g"), v8::Number::New(isolate, col.g));
            ret->Set(v8::String::NewFromUtf8(isolate, "b"), v8::Number::New(isolate, col.b));
            ret->Set(v8::String::NewFromUtf8(isolate, "a"), v8::Number::New(isolate, col.b));
            
            return ret;
        }
        
        ENGINE_JS_METHOD(GetColor) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_JS_SCOPE_CLOSE(_getColor(args.GetIsolate(), GetDraw2D(args.This())));
        }
        
        ENGINE_JS_METHOD(GetRGBFromHSV) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Isolate* isolate = args.GetIsolate();
            
            ENGINE_CHECK_ARGS_LENGTH(3);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the Hue Component between 0 and 360");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 is the Saturation Component between 0.0f and 1.0f");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 is the Value Component between 0.0f and 1.0f");
            
            v8::Handle<v8::Object> ret = v8::Object::New(isolate);
            
            float hue = ENGINE_GET_ARG_NUMBER_VALUE(0),
            saturation = ENGINE_GET_ARG_NUMBER_VALUE(1),
            value = ENGINE_GET_ARG_NUMBER_VALUE(2);
            
            Color4f col = Color4f::FromHSV(hue, saturation, value);
            
            ret->Set(v8::String::NewFromUtf8(isolate, "r"), v8::Number::New(isolate, col.r));
            ret->Set(v8::String::NewFromUtf8(isolate, "g"), v8::Number::New(isolate, col.g));
            ret->Set(v8::String::NewFromUtf8(isolate, "b"), v8::Number::New(isolate, col.b));
            ret->Set(v8::String::NewFromUtf8(isolate, "a"), v8::Number::New(isolate,col.a));
            
            ENGINE_JS_SCOPE_CLOSE(ret);
        }
        
        ENGINE_JS_METHOD(ClearColor) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Isolate* isolate = args.GetIsolate();
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            if (args[0]->IsNumber()) {
                int col = ENGINE_GET_ARG_INT32_VALUE(0);
                
                if (col > (256 * 256 * 256))
                {
                    ENGINE_THROW_ARGERROR("Arg0 is beyond 0xffffff");
                    ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
                }
                
                GetDraw2D(args.This())->GetRender()->ClearColor(col); // yay now draw2d handles it
            } else if (args[0]->IsString()) {
                GetDraw2D(args.This())->GetRender()->ClearColor(ENGINE_GET_ARG_CPPSTRING_VALUE(0));
            } else if (args[0]->IsObject()) {
                v8::Local<v8::Object> obj = ENGINE_GET_ARG_OBJECT(0);
                double r = obj->Get(v8::String::NewFromUtf8(isolate, "r"))->NumberValue();
                double g = obj->Get(v8::String::NewFromUtf8(isolate, "g"))->NumberValue();
                double b = obj->Get(v8::String::NewFromUtf8(isolate, "b"))->NumberValue();
                GetDraw2D(args.This())->GetRender()->ClearColor(r, g, b);
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
            
            GetDraw2D(args.This())->GetRender()->LoadFont(ENGINE_GET_ARG_CPPSTRING_VALUE(0), ENGINE_GET_ARG_CPPSTRING_VALUE(1));
            
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
            
            GetDraw2D(args.This())->GetRender()->SetFont(ENGINE_GET_ARG_CPPSTRING_VALUE(0), ENGINE_GET_ARG_NUMBER_VALUE(1));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(IsFontLoaded) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the name of the font to check");
            
            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(args.GetIsolate(), GetDraw2D(args.This())->GetRender()->IsFontLoaded(ENGINE_GET_ARG_CPPSTRING_VALUE(0))));
        }
        
        ENGINE_JS_METHOD(Print) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(3);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the X position of Arg2");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 is the Y position of Arg2");
            ENGINE_CHECK_ARG_STRING(2, "Arg2 is the string to print");
            
            std::string str = ENGINE_GET_ARG_CPPSTRING_VALUE(2);
            
            GetDraw2D(args.This())->GetRender()->Print(ENGINE_GET_ARG_NUMBER_VALUE(0), ENGINE_GET_ARG_NUMBER_VALUE(1), (const char*) str.c_str());
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(GetStringWidth) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the string to get the width of");
            
            ENGINE_JS_SCOPE_CLOSE(v8::Number::New(args.GetIsolate(), GetDraw2D(args.This())->GetRender()->CalcStringWidth(ENGINE_GET_ARG_CPPSTRING_VALUE(0))));
        }
        
        // Texture Handling
        
        ENGINE_JS_METHOD(CreateTexture) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(Draw) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            GetDraw2D(args.This())->GetRender()->CheckError("JSDraw::Draw::PreDraw");
            
            TexturePtr tex;
            float x, y, w, h;
            
            ENGINE_CHECK_ARGS_LENGTH(5);
            
            ENGINE_CHECK_ARG_EXTERNAL(0, "Arg0 is a valid texture that's been loaded since the last context change");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 has to be X of a rect");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 has to be Y of a rect");
            ENGINE_CHECK_ARG_NUMBER(3, "Arg3 has to be Width of a rect");
            ENGINE_CHECK_ARG_NUMBER(4, "Arg4 has to be Height of a rect");
            
            tex = (TexturePtr)ENGINE_GET_ARG_EXTERNAL_VALUE(0);
            
            if (!tex->IsValid()) {
                ENGINE_THROW_ARGERROR("Arg0 is not a valid texture");
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            x = (float)ENGINE_GET_ARG_NUMBER_VALUE(1);
            y = (float)ENGINE_GET_ARG_NUMBER_VALUE(2);
            w = (float)ENGINE_GET_ARG_NUMBER_VALUE(3);
            h = (float)ENGINE_GET_ARG_NUMBER_VALUE(4);
            
            GetDraw2D(args.This())->DrawImage(tex, x, y, w, h);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(DrawSub) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            GetDraw2D(args.This())->GetRender()->CheckError("Pre Image Draw");
            
            TexturePtr tex;
            float x1, y1, w1, h1,
            x2, y2, w2, h2;
            int imageWidth, imageHeight;
            
            ENGINE_CHECK_ARGS_LENGTH(9);
            
            ENGINE_CHECK_ARG_EXTERNAL(0, "Arg0 is a valid texture that's been loaded since the last context change");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 has to be X of a rect");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 has to be Y of a rect");
            ENGINE_CHECK_ARG_NUMBER(3, "Arg3 has to be Width of a rect");
            ENGINE_CHECK_ARG_NUMBER(4, "Arg4 has to be Height of a rect");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg5 has to be X of a sub rectangle");
            ENGINE_CHECK_ARG_NUMBER(2, "Arg6 has to be Y of a sub rectangle");
            ENGINE_CHECK_ARG_NUMBER(3, "Arg7 has to be Width of a sub rectangle");
            ENGINE_CHECK_ARG_NUMBER(4, "Arg8 has to be Height of a sub rectangle");
            
            tex = (TexturePtr)ENGINE_GET_ARG_EXTERNAL_VALUE(0);
            
            if (!tex->IsValid()) {
                ENGINE_THROW_ARGERROR("Arg0 is not a valid texture");
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            x1 = (float)ENGINE_GET_ARG_NUMBER_VALUE(1);
            y1 = (float)ENGINE_GET_ARG_NUMBER_VALUE(2);
            w1 = (float)ENGINE_GET_ARG_NUMBER_VALUE(3);
            h1 = (float)ENGINE_GET_ARG_NUMBER_VALUE(4);
            x2 = (float)ENGINE_GET_ARG_NUMBER_VALUE(5);
            y2 = (float)ENGINE_GET_ARG_NUMBER_VALUE(6);
            w2 = (float)ENGINE_GET_ARG_NUMBER_VALUE(7);
            h2 = (float)ENGINE_GET_ARG_NUMBER_VALUE(8);
            
            GetDraw2D(args.This())->DrawImage(tex, x1, y1, w1, h1, x2, y2, w2, h2);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(DrawSprite) {
            ENGINE_JS_SCOPE_OPEN;
            
            SpriteSheetPtr sheet;
            std::string sprite;
            float x, y, w, h;
            
            ENGINE_CHECK_ARGS_LENGTH(6);
            
            ENGINE_CHECK_ARG_EXTERNAL(0, "Arg0 is a valid spritesheet that's been loaded since the last context change");
            ENGINE_CHECK_ARG_STRING(1, "Arg1 is the sprite or animation to draw")
            ENGINE_CHECK_ARG_NUMBER(2, "Arg2 has to be X of a rect");
            ENGINE_CHECK_ARG_NUMBER(3, "Arg3 has to be Y of a rect");
            ENGINE_CHECK_ARG_NUMBER(4, "Arg4 has to be Width of a rect");
            ENGINE_CHECK_ARG_NUMBER(5, "Arg5 has to be Height of a rect");
            
            sheet = (SpriteSheetPtr)ENGINE_GET_ARG_EXTERNAL_VALUE(0);
            
            if (!sheet->IsValid()) {
                ENGINE_THROW_ARGERROR("Arg0 is not a valid spritesheet");
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            sprite = ENGINE_GET_ARG_CPPSTRING_VALUE(1);
            x = (float)ENGINE_GET_ARG_NUMBER_VALUE(2);
            y = (float)ENGINE_GET_ARG_NUMBER_VALUE(3);
            w = (float)ENGINE_GET_ARG_NUMBER_VALUE(4);
            h = (float)ENGINE_GET_ARG_NUMBER_VALUE(5);
            
            GetDraw2D(args.This())->DrawSprite(sheet, sprite, x, y, w, h);
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(OpenImage) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the filename of the image to load");
            
            GetDraw2D(args.This())->GetRender()->CheckError("Pre Image Load");
            
            if (!Filesystem::FileExists(ENGINE_GET_ARG_CPPSTRING_VALUE(0))) {
                ENGINE_THROW_ARGERROR("File does not Exist");
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            std::string filename = ENGINE_GET_ARG_CPPSTRING_VALUE(0);
            
            if (!ResourceManager::HasSource(filename)) {
                ResourceManager::Load(filename);
            }
            
            ResourceManager::ImageResourcePtr img = new ResourceManager::ImageResource(filename);
            
            img->Load();
            
            ENGINE_JS_SCOPE_CLOSE(v8::External::New(args.GetIsolate(), img->GetTexture()));
        }
        
        ENGINE_JS_METHOD(OpenSpriteSheet) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the filename of the scriptsheet json to load");
            
            std::string filename = ENGINE_GET_ARG_CPPSTRING_VALUE(0);
            
            if (!ResourceManager::HasSource(filename)) {
                ResourceManager::Load(filename);
            }
            
            ENGINE_JS_SCOPE_CLOSE(v8::External::New(args.GetIsolate(), SpriteSheetReader::LoadSpriteSheetFromFile(filename)));
        }
        
        ENGINE_JS_METHOD(GetImageArray) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the filename to load");
            
            v8::Handle<v8::Object> array = v8::Object::New(args.GetIsolate());
            
            v8::Isolate* isolate = v8::Isolate::GetCurrent();
            
            long fileSize = 0;
            
            unsigned char* file = (unsigned char*) Filesystem::GetFileContent(ENGINE_GET_ARG_CPPSTRING_VALUE(0), fileSize);
            
            int imageWidth, imageHeight, chaneals;
            
            unsigned char* pixel = SOIL_load_image_from_memory(file, fileSize, &imageWidth, &imageHeight, &chaneals, SOIL_LOAD_RGBA);
            
            int imageSize = imageWidth * imageHeight * 4;
            
            float* rawArray = new float[imageSize];
            
            for (int i = 0; i < imageSize; i += 4) {
                rawArray[i] = 1.0f;
                rawArray[i + 1] = 1.0f;
                rawArray[i + 2] = 1.0f;
                rawArray[i + 3] = 1.0f;
            }
            
            v8::Persistent<v8::Object> arr;
            
            arr.Reset(isolate, array);
            
            arr.MarkIndependent();
            arr.SetWeak(rawArray, ReadBufferWeakCallback);
            
            isolate->AdjustAmountOfExternalAllocatedMemory(imageSize * sizeof(float));
            
            array->SetIndexedPropertiesToExternalArrayData(rawArray, v8::kExternalFloatArray, imageSize * sizeof(float));
            
            array->Set(v8::String::NewFromUtf8(isolate, "length"), v8::Number::New(isolate, imageSize));
            array->Set(v8::String::NewFromUtf8(isolate, "width"), v8::Number::New(isolate, imageWidth));
            array->Set(v8::String::NewFromUtf8(isolate, "height"), v8::Number::New(isolate, imageHeight));
            array->Set(v8::String::NewFromUtf8(isolate, "byteLength"),
                       v8::Int32::New(isolate, static_cast<int32_t>(imageSize)), v8::ReadOnly);
            
            int pos = 0;
            int i = 0;
            
            for (int x = 0; x < imageWidth; x++) {
                for (int y = 0; y < imageHeight; y++) {
                    array->Set(i + 0, v8::Number::New(isolate, (float) pixel[pos + 2] / 256.f));
                    array->Set(i + 1, v8::Number::New(isolate, (float) pixel[pos + 1] / 256.f));
                    array->Set(i + 2, v8::Number::New(isolate, (float) pixel[pos + 0] / 256.f));
                    array->Set(i + 3, v8::Number::New(isolate, 1.0f));
                    pos += 4;
                    i += 4;
                }
            }
            
            SOIL_free_image_data(pixel);
            
            ENGINE_JS_SCOPE_CLOSE(array);
        }
        
        ENGINE_JS_METHOD(CreateImageArray) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_INT32(0, "Arg0 is the width of the new Image Array");
            ENGINE_CHECK_ARG_INT32(1, "Arg1 is the height of the new Image Array");
            
            int imageWidth = ENGINE_GET_ARG_INT32_VALUE(0),
            imageHeight = ENGINE_GET_ARG_INT32_VALUE(1);
            
            int arraySize = imageWidth * imageHeight * 4;
            
            v8::Isolate* isolate = v8::Isolate::GetCurrent();
            
            v8::Handle<v8::Object> array = v8::Object::New(isolate);
            
            float* rawArray = new float[arraySize]; // sadly this is a memory leak right now
            
            for (int i = 0; i < arraySize; i += 4) {
                rawArray[i] = 1.0f;
                rawArray[i + 1] = 1.0f;
                rawArray[i + 2] = 1.0f;
                rawArray[i + 3] = 1.0f;
            }
            
            v8::Persistent<v8::Object> arr;
            
            arr.Reset(isolate, array);
            
            arr.MarkIndependent();
            arr.SetWeak(rawArray, ReadBufferWeakCallback);
            
            isolate->AdjustAmountOfExternalAllocatedMemory(arraySize * sizeof(float));
            
            array->SetIndexedPropertiesToExternalArrayData(rawArray, v8::kExternalFloatArray, arraySize * sizeof(float));
            
            array->Set(v8::String::NewFromUtf8(isolate, "length"), v8::Number::New(isolate, arraySize));
            array->Set(v8::String::NewFromUtf8(isolate, "width"), v8::Number::New(isolate, imageWidth));
            array->Set(v8::String::NewFromUtf8(isolate, "height"), v8::Number::New(isolate, imageHeight));
            array->Set(v8::String::NewFromUtf8(isolate, "byteLength"),
                       v8::Int32::New(isolate, static_cast<int32_t>(arraySize)), v8::ReadOnly);
            
            ENGINE_JS_SCOPE_CLOSE(array);
        }
        
        ENGINE_JS_METHOD(CreateImage) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            if (args.Length() < 3) {
                ENGINE_THROW_ARGERROR("Wrong number of args draw.createImage takes 3 or 4 args");
            }
            
            ENGINE_CHECK_ARG_OBJECT(0, "Arg0 is a Object");
            ENGINE_CHECK_ARG_INT32(1, "Arg1 is the width of the new image");
            ENGINE_CHECK_ARG_INT32(2, "Arg2 is the height of the new image");
            
            unsigned int texID = 0;
            
            if (args.Length() == 4) {
                ENGINE_CHECK_ARG_INT32(3, "Arg3 is the texture ID to reuse");
                texID = ENGINE_GET_ARG_INT32_VALUE(3);
            }
            
            v8::Local<v8::Array> arr = ENGINE_GET_ARG_ARRAY(0);
            
            int width = ENGINE_GET_ARG_INT32_VALUE(1);
            int height = ENGINE_GET_ARG_INT32_VALUE(2);
            
            float* pixels = NULL;
            
            if (arr->HasIndexedPropertiesInExternalArrayData()) {
                pixels = (float*) arr->GetIndexedPropertiesExternalArrayData(); // yay fast
            } else {
                int len = width * height;
                
                bool noClamp = !Config::GetBoolean("core.render.clampTexture");
                
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
            
            TexturePtr t = ImageReader::TextureFromBuffer(texID, pixels, width, height);
            
            if (!arr->HasIndexedPropertiesInExternalArrayData()) {
                free(pixels);
            }
            
            ENGINE_JS_SCOPE_CLOSE(v8::External::New(args.GetIsolate(), t));
        }
        
        ENGINE_JS_METHOD(SaveImage) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            ENGINE_CHECK_ARG_EXTERNAL(0, "Arg0 is the index of the image returned from draw.createImage or draw.openImage");
            ENGINE_CHECK_ARG_STRING(1, "Arg1 is the filename to save the image as");
            
            TexturePtr tex = (TexturePtr)ENGINE_GET_ARG_EXTERNAL_VALUE(0);
            
            if (!tex->IsValid()) {
                ENGINE_THROW_ARGERROR("Arg0 is not a valid textureID");
                ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
            }
            
            tex->Save(ENGINE_GET_ARG_CPPSTRING_VALUE(1));
            
            Logger::begin("SaveImage", Logger::LogLevel_Log)
            << "Saved image as: " << Filesystem::GetRealPath(ENGINE_GET_ARG_CPPSTRING_VALUE(1))
            << Logger::end();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(FreeImage) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_EXTERNAL(0, "Arg0 is the texture to free");
            
            TexturePtr tex = (Texture*)ENGINE_GET_ARG_EXTERNAL_VALUE(0);
            
            tex->Invalidate();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(IsTexture) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            if (!args[0]->IsExternal()) {
                ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(args.GetIsolate(), false));
            }
            
            TexturePtr tex = (TexturePtr)ENGINE_GET_ARG_EXTERNAL_VALUE(0);
            
            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(args.GetIsolate(), tex->IsValid()));
        }
        
        ENGINE_JS_METHOD(IsSpriteSheet) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            if (!args[0]->IsExternal()) {
                ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(args.GetIsolate(), false));
            }
            
            SpriteSheetPtr sprite = (SpriteSheetPtr)ENGINE_GET_ARG_EXTERNAL_VALUE(0);
            
            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(args.GetIsolate(), sprite->IsValid()));
        }
        
        // basicly restarts 2d drawing
        ENGINE_JS_METHOD(CameraReset) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            GetDraw2D(args.This())->GetRender()->Reset();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(CameraPan) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the X Distince to pan");
            ENGINE_CHECK_ARG_NUMBER(1, "Arg1 is the Y Distince to pan");
            
            GetDraw2D(args.This())->GetRender()->CameraPan(ENGINE_GET_ARG_NUMBER_VALUE(0), ENGINE_GET_ARG_NUMBER_VALUE(1));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(CameraZoom) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the factor to zoom the camera");
            
            GetDraw2D(args.This())->GetRender()->CameraZoom(ENGINE_GET_ARG_NUMBER_VALUE(0));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(CameraRotate) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_GL;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is the factor to rotate the camera");
            
            GetDraw2D(args.This())->GetRender()->CameraRotate(ENGINE_GET_ARG_NUMBER_VALUE(0));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(SetCenter) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_INT32(0, "Arg0 is the x to offset drawing by");
            ENGINE_CHECK_ARG_INT32(1, "Arg1 is the y to offset drawing by");
            
            GetDraw2D(args.This())->GetRender()->SetCenter(ENGINE_GET_ARG_INT32_VALUE(0),
                                                           ENGINE_GET_ARG_INT32_VALUE(1));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        void DrawColorGetter(v8::Local<v8::String> prop, const v8::PropertyCallbackInfo<v8::Value>& info) {
            info.GetReturnValue().Set(_getColor(info.GetIsolate(), GetDraw2D(info.Holder())));
        }
        
        void DrawColorSetter(v8::Local<v8::String> prop, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
            _setColor(info.GetIsolate(), GetDraw2D(info.Holder()), value);
        }
        
#define addItem(table, js_name, funct) table->Set(isolate, js_name, v8::FunctionTemplate::New(isolate, funct))
        
        void InitDraw(v8::Handle<v8::ObjectTemplate> drawTable) {
            InitColorObject(drawTable);
            InitVertexBuffer2DObject(drawTable);
            
            v8::Isolate* isolate = v8::Isolate::GetCurrent();
            
            addItem(drawTable, "rect", Rect);
            addItem(drawTable, "grid", Grid);
            addItem(drawTable, "grad", Grad);
            addItem(drawTable, "circle", Circle);
            addItem(drawTable, "curve", Curve);
            addItem(drawTable, "line", Line);
            addItem(drawTable, "polygon", Polygon);
            
            addItem(drawTable, "colorPalette", ColorPalette);
            addItem(drawTable, "setColorF", SetColorF);
            addItem(drawTable, "setColor", SetColor);
            addItem(drawTable, "setColorI", SetColorI);
            addItem(drawTable, "getColor", GetColor);
            addItem(drawTable, "clearColor", ClearColor);
            addItem(drawTable, "getRGBFromHSV", GetRGBFromHSV);
            
            addItem(drawTable, "print", Print);
            addItem(drawTable, "getStringWidth", GetStringWidth);
            
            addItem(drawTable, "draw", Draw);
            addItem(drawTable, "drawSub", DrawSub);
            addItem(drawTable, "drawSprite", DrawSprite);
            addItem(drawTable, "openImage", OpenImage);
            addItem(drawTable, "openSpriteSheet", OpenSpriteSheet);
            addItem(drawTable, "getImageArray", GetImageArray);
            addItem(drawTable, "createImageArray", CreateImageArray);
            addItem(drawTable, "createImage", CreateImage);
            addItem(drawTable, "saveImage", SaveImage);
            addItem(drawTable, "freeImage", FreeImage);
            addItem(drawTable, "isTexture", IsTexture);
            addItem(drawTable, "isSpriteSheet", IsSpriteSheet);
            
            addItem(drawTable, "cameraReset", CameraReset);
            addItem(drawTable, "cameraPan", CameraPan);
            addItem(drawTable, "cameraZoom", CameraZoom);
            addItem(drawTable, "cameraRotate", CameraRotate);
            
            addItem(drawTable, "setFont", SetFont);
            addItem(drawTable, "loadFont", LoadFont);
            addItem(drawTable, "isFontLoaded", IsFontLoaded);
            
            addItem(drawTable, "setCenter", SetCenter);
            
            drawTable->SetAccessor(v8::String::NewFromUtf8(isolate, "drawColor"), DrawColorGetter, DrawColorSetter);
        }
        
#undef addItem
        
    } // namespace JsDraw
} // namespace Engine