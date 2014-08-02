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
        
        v8::Persistent<v8::FunctionTemplate> colorTemplate, vertexBuffer2DTemplate;
        
        Color4f Color4fFromObject(v8::Isolate* isolate, v8::Handle<v8::Object> obj) {
            Color4f col;
            col.r = obj->Get(v8::String::NewFromUtf8(isolate, "r"))->NumberValue();
            col.g = obj->Get(v8::String::NewFromUtf8(isolate, "g"))->NumberValue();
            col.b = obj->Get(v8::String::NewFromUtf8(isolate, "b"))->NumberValue();
            col.a = obj->Get(v8::String::NewFromUtf8(isolate, "a"))->NumberValue();
            return col;
        }
        
        v8::Local<v8::Object> Color4fToObject(v8::Isolate* isolate, Color4f col) {
            std::vector<v8::Handle<v8::Value>> av(4, v8::Undefined(isolate));
            
            av[0] = v8::Number::New(isolate, col.r);
            av[1] = v8::Number::New(isolate, col.g);
            av[2] = v8::Number::New(isolate, col.b);
            av[3] = v8::Number::New(isolate, col.a);
            
            v8::Local<v8::FunctionTemplate> func = v8::Local<v8::FunctionTemplate>::New(isolate, colorTemplate);
            return func->GetFunction()->NewInstance(4, &av[0]);
        }
        
        void CreateColor(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.RecallAsConstructor()) return;
            
            Color4f col;
            
            if (args.Length() == 0) { // new Color()
                col = Color4f();
            } else if (args.Length() == 1) { // new Color(number(0xrrggbb) || string('predefined'))
                if (args[0]->IsNumber()) {
                    col = Color4f(args.Int32Value(0));
                } else if (args[0]->IsString()) {
                    col = Color4f(args.StringValue(0));
                } else {
                    col = Color4f();
                }
            } else if (args.Length() == 2) { // new Color(number(0xrrggbb), number(alpha))
                col = Color4f(args.Int32Value(0));
                col.a = args.NumberValue(1);
            } else if (args.Length() == 3) { // new Color(number(r), number(g), number(b))
                col = Color4f(args.NumberValue(0),
                              args.NumberValue(1),
                              args.NumberValue(2),
                              1.0f);
            } else if (args.Length() == 4) { // new Color(number(r), number(g), number(b), number(a))
                col = Color4f(args.NumberValue(0),
                              args.NumberValue(1),
                              args.NumberValue(2),
                              1.0f);
            } else { // undefined
                col = Color4f();
            }
            
            args.This()->Set(args.NewString("r"), args.NewNumber(col.r));
            args.This()->Set(args.NewString("g"), args.NewNumber(col.g));
            args.This()->Set(args.NewString("b"), args.NewNumber(col.b));
            args.This()->Set(args.NewString("a"), args.NewNumber(col.a));
        }
        
        void Color_FromHSV(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            args.AssertCount(3);
            
            args.Assert(args[0]->IsNumber(), "Arg0 is the Hue Component between 0 and 360");
            args.Assert(args[1]->IsNumber(), "Arg1 is the Saturation Component between 0.0f and 1.0f");
            args.Assert(args[2]->IsNumber(), "Arg2 is the Value Component between 0.0f and 1.0f");
            
            v8::Handle<v8::Object> ret = args.NewObject();
            
            float hue = args.NumberValue(0),
            saturation = args.NumberValue(1),
            value = args.NumberValue(2);
            
            Color4f col = Color4f::FromHSV(hue, saturation, value);
            
            args.SetReturnValue(Color4fToObject(args.GetIsolate(), col));
        }
        
        void Color_FromRandom(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            static BasicRandom rand = BasicRandom();
            
            args.SetReturnValue(Color4fToObject(args.GetIsolate(), Color4f(rand.NextDouble(), rand.NextDouble(), rand.NextDouble(), 1.0f)));
        }
        
        void Color_ToString(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            std::stringstream sb;
            
            sb << "[color rgba(";
            
            sb << std::floor(args.This()->Get(args.NewString("r"))->NumberValue() * 255) << ", ";
            sb << std::floor(args.This()->Get(args.NewString("g"))->NumberValue() * 255) << ", ";
            sb << std::floor(args.This()->Get(args.NewString("b"))->NumberValue() * 255) << ", ";
            sb << std::floor(args.This()->Get(args.NewString("a"))->NumberValue() * 255);
            
            sb << ") ]";
            
            args.SetReturnValue(args.NewString(sb.str()));
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
            
            colorTemplate.Reset(isolate, newColor);
            
            drawTable->Set(isolate, "Color", newColor);
        }
        
        class JS_VertexBuffer2D : public GL3Buffer, public ScriptingManager::ObjectWrap {
        public:
        
            static void Create(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                if (args.RecallAsConstructor()) return;
                
                args.AssertCount(1);
                
                args.Assert(args[0]->IsString(), "Arg0 is the Effect filename to use");
                
                RenderDriverPtr render = GetAppSingilton()->GetRender();
                
                JS_VertexBuffer2D::Wrap<JS_VertexBuffer2D>(args.GetIsolate(), args.This());
                
                if (render->GetRendererType() == RendererType::OpenGL3) {
                    EffectParametersPtr effect = EffectReader::GetEffectFromFile(args.StringValue(0));
                    effect->CreateShader();
                    JS_VertexBuffer2D::Unwarp<JS_VertexBuffer2D>(args.This())->GL3Buffer::Init(render, effect);
                }
            }
            
            static void AddVert(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                glm::vec3 pos;
                Color4f col = Color4f("white");
                glm::vec2 uv;
                
                if (args.Length() >= 2) { // (x, y)
                    pos = glm::vec3(args.NumberValue(0),
                                    args.NumberValue(1),
                                    0.0f);
                }
                if (args.Length() >= 3) { // (x, y, col)
                    v8::Handle<v8::Object> obj = args[2]->ToObject();
                    col = Color4fFromObject(args.GetIsolate(), obj);
                }
                if (args.Length() >= 5) { // (x, y, col, u, v)
                    uv = glm::vec2(args.NumberValue(3),
                                   args.NumberValue(4));
                }
                
                JS_VertexBuffer2D::Unwarp<JS_VertexBuffer2D>(args.This())->GL3Buffer::AddVert(pos, col, uv);
            }
            
            static void Draw(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                JS_VertexBuffer2D::Unwarp<JS_VertexBuffer2D>(args.This())->GL3Buffer::Draw(PolygonMode::Triangles, glm::mat4(), glm::mat4());
            }
            
            static void Save(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                if (args.AssertCount(1)) return;
                
                if (args.Assert(args[0]->IsString(), "Arg0 is the filename to save to")) return;
                
                JS_VertexBuffer2D::Unwarp<JS_VertexBuffer2D>(args.This())->GL3Buffer::Save(args.StringValue(0));
            }
            
            static void Load(const v8::FunctionCallbackInfo<v8::Value>& _args) {
                ScriptingManager::Arguments args(_args);
                
                if (args.AssertCount(1)) return;
                
                if (args.Assert(args[0]->IsString(), "Arg0 is the filename to load from")) return;
                
                JS_VertexBuffer2D::Unwarp<JS_VertexBuffer2D>(args.This())->GL3Buffer::Load(args.StringValue(0));
            }
            
            static void Init(v8::Handle<v8::ObjectTemplate> drawTable) {
                ScriptingManager::Factory f(v8::Isolate::GetCurrent());
                v8::HandleScope scope(f.GetIsolate());
                
                v8::Handle<v8::FunctionTemplate> newVertexBuffer = f.NewFunctionTemplate(Create);
                
                newVertexBuffer->SetClassName(f.NewString("VertexBuffer2D"));
                
                f.FillTemplate(newVertexBuffer, {
                    {FTT_Prototype, "addVert", f.NewFunctionTemplate(AddVert)},
                    {FTT_Prototype, "draw", f.NewFunctionTemplate(Draw)},
                    {FTT_Prototype, "save", f.NewFunctionTemplate(Save)},
                    {FTT_Prototype, "load", f.NewFunctionTemplate(Load)}
                });
                
                newVertexBuffer->InstanceTemplate()->SetInternalFieldCount(1);
                
                vertexBuffer2DTemplate.Reset(f.GetIsolate(), newVertexBuffer);
                
                drawTable->Set(f.GetIsolate(), "VertexBuffer2D", newVertexBuffer);
            }
        };
        
        inline Draw2DPtr GetDraw2D(v8::Local<v8::Object> thisValue) {
            return (Draw2DPtr) thisValue->GetHiddenValue(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "_draw")).As<v8::External>()->Value();
        }
        
		void Rect(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(4)) return;
            
            if (args.Assert(args[0]->IsNumber(), "Arg0 has to be X of a rect") ||
                args.Assert(args[1]->IsNumber(), "Arg1 has to be Y of a rect") ||
                args.Assert(args[2]->IsNumber(), "Arg2 has to be Width of a rect") ||
                args.Assert(args[3]->IsNumber(), "Arg3 has to be Height of a rect")) return;
            
            GetDraw2D(args.This())->Rect(args.NumberValue(0),
                                         args.NumberValue(1),
                                         args.NumberValue(2),
                                         args.NumberValue(3));
		}
		
		void Grid(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(4)) return;
            
            if (args.Assert(args[0]->IsNumber(), "Arg0 has to be X of a grid") ||
                args.Assert(args[1]->IsNumber(), "Arg1 has to be Y of a grid") ||
                args.Assert(args[2]->IsNumber(), "Arg2 has to be Width of a grid") ||
                args.Assert(args[3]->IsNumber(), "Arg3 has to be Height of a grid")) return;
            
            GetDraw2D(args.This())->Grid(args.NumberValue(0),
                                         args.NumberValue(1),
                                         args.NumberValue(2),
                                         args.NumberValue(3));
		}
		
		void Grad(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(7)) return;
            
            if (args.Assert(args[0]->IsNumber(), "Arg0 has to be X of a gradient") ||
                args.Assert(args[1]->IsNumber(), "Arg1 has to be Y of a gradient") ||
                args.Assert(args[2]->IsNumber(), "Arg2 has to be Width of a gradient") ||
                args.Assert(args[3]->IsNumber(), "Arg3 has to be Height of a gradient") ||
                args.Assert(args[4]->IsInt32(), "Arg4 is Color1 of the gradient") ||
                args.Assert(args[5]->IsInt32(), "Arg5 is Color2 of the gradient") ||
                args.Assert(args[6]->IsBoolean(), "Arg6 set's orientation of the gradient")) return;
            
            GetDraw2D(args.This())->Grad(args.NumberValue(0),
                                         args.NumberValue(1),
                                         args.NumberValue(2),
                                         args.NumberValue(3),
                                         args.Int32Value(4),
                                         args.Int32Value(5),
                                         args.BooleanValue(6));
		}
        
        void Circle(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.Assert(args[0]->IsNumber(), "Arg0 is the X center of the circle") ||
                args.Assert(args[1]->IsNumber(), "Arg1 is the Y center of the circle") ||
                args.Assert(args[2]->IsNumber(), "Arg2 is the radius of the circle")) return;
            
            if (args.Length() == 4) {
                if (args.Assert(args[3]->IsBoolean(), "Arg3 sets the fill style of the circle")) return;
                GetDraw2D(args.This())->Circle(args.NumberValue(0),
                                               args.NumberValue(1),
                                               args.NumberValue(2),
                                               args.BooleanValue(3));
            } else if (args.Length() == 5) {
                if (args.Assert(args[3]->IsNumber(), "Arg3 is the number of sides to the circle") ||
                    args.Assert(args[4]->IsBoolean(), "Arg4 sets the fill style of the circle")) return;
                GetDraw2D(args.This())->Circle(args.NumberValue(0),
                                               args.NumberValue(1),
                                               args.NumberValue(2),
                                               args.NumberValue(3),
                                               args.BooleanValue(4));
            } else if (args.Length() == 7) {
                if (args.Assert(args[3]->IsNumber(), "Arg3 is the number of sides to the circle") ||
                    args.Assert(args[4]->IsNumber(), "Arg4 is the start % of the circle") ||
                    args.Assert(args[5]->IsNumber(), "Arg5 is the end % of the circle") ||
                    args.Assert(args[6]->IsBoolean(), "Arg6 sets the fill style of the circle")) return;
                GetDraw2D(args.This())->Circle(args.NumberValue(0),
                                               args.NumberValue(1),
                                               args.NumberValue(2),
                                               args.NumberValue(3),
                                               args.NumberValue(4),
                                               args.NumberValue(5),
                                               args.BooleanValue(6));
            } else if (args.Length() == 8) {
                if (args.Assert(args[3]->IsNumber(), "Arg3 is the inner radius of the circle") ||
                    args.Assert(args[4]->IsNumber(), "Arg4 is the number of sides to the circle") ||
                    args.Assert(args[5]->IsNumber(), "Arg5 is the start % of the circle") ||
                    args.Assert(args[6]->IsNumber(), "Arg6 is the end % of the circle") ||
                    args.Assert(args[7]->IsBoolean(), "Arg7 sets the fill style of the circle")) return;
                GetDraw2D(args.This())->Circle(args.NumberValue(0),
                                               args.NumberValue(1),
                                               args.NumberValue(2),
                                               args.NumberValue(3),
                                               args.NumberValue(4),
                                               args.NumberValue(5),
                                               args.NumberValue(6),
                                               args.BooleanValue(7));
            } else {
                args.ThrowError("Wrong number of Arguments");
            }
        }
        
        void Line(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(4)) return;
            
            if (args.Assert(args[0]->IsNumber(), "Arg0 has to be the first X point of the line") ||
                args.Assert(args[1]->IsNumber(), "Arg1 has to be the first Y point of the line") ||
                args.Assert(args[2]->IsNumber(), "Arg2 has to be the second X point of the line") ||
                args.Assert(args[3]->IsNumber(), "Arg3 has to be the second Y point of the line")) return;
            
            GetDraw2D(args.This())->Line(args.NumberValue(0),
                                         args.NumberValue(1),
                                         args.NumberValue(2),
                                         args.NumberValue(3));
        }
        
        void Polygon(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(3)) return;
            
            if (args.Assert(args[0]->IsNumber(), "Arg0 is the X center of the polygon") ||
                args.Assert(args[1]->IsNumber(), "Arg1 is the Y center of the polygon") ||
                args.Assert(args[2]->IsArray(), "Arg2 is the points to the circle in the format [x, y, x, y]")) return;
            
            v8::Handle<v8::Array> arr = args[2].As<v8::Array>();
            
            float* circlePoints = new float[arr->Length()];
            int circlePointCount = arr->Length() / 2;
            
            for (int i = 0; i < arr->Length(); i++) {
                circlePoints[i] = arr->Get(i).As<v8::Number>()->Value();
            }
            
            GetDraw2D(args.This())->Polygon(args.NumberValue(0), args.NumberValue(1), circlePoints, circlePointCount);
        }
        
        void Curve(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(8)) return;
            
            if (args.Assert(args[0]->IsNumber(), "Arg0 is the x value of point 1") ||
                args.Assert(args[1]->IsNumber(), "Arg1 is the y value of point 1") ||
                args.Assert(args[2]->IsNumber(), "Arg2 is the x value of point 2") ||
                args.Assert(args[3]->IsNumber(), "Arg3 is the y value of point 2") ||
                args.Assert(args[4]->IsNumber(), "Arg4 is the x value of point 3") ||
                args.Assert(args[5]->IsNumber(),  "Arg5 is the y value of point 3") ||
                args.Assert(args[6]->IsNumber(), "Arg6 is the x value of point 4") ||
                args.Assert(args[7]->IsNumber(), "Arg7 is the y value of point 4")) return;
            
            GetDraw2D(args.This())->BezierCurve(args.NumberValue(0),
                                                args.NumberValue(1),
                                                args.NumberValue(2),
                                                args.NumberValue(3),
                                                args.NumberValue(4),
                                                args.NumberValue(5),
                                                args.NumberValue(6),
                                                args.NumberValue(7));
        }
		
        void ColorPalette(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Length() == 2) {
                if (args.Assert(args[0]->IsString(), "Arg0 is a name for the color") ||
                    args.Assert(args[1]->IsInt32(), "Arg1 is the color to associate the name with")) return;
                
                Color4f::SetDefinedColor(args.StringValue(0), args.Int32Value(1));
            } else if (args.Length() == 1) {
                if (args.Assert(args[0]->IsObject(), "Arg0 is an object containing a list of colors")) return;
                
                v8::Local<v8::Object> obj = args[0]->ToObject();
                
                v8::Local<v8::Array> objNames = obj->GetPropertyNames();
                
                for (int i = 0; i < objNames->Length(); i++) {
                    v8::Local<v8::String> objKey = objNames->Get(i)->ToString();
                    v8::Local<v8::Value> objItem = obj->Get(objKey);
                    
                    Color4f::SetDefinedColor(std::string(*v8::String::Utf8Value(objKey)), (int) objItem->NumberValue());
                }
            } else {
                args.ThrowArgError("draw.colorPalette takes 1 or 2 args");
            }
        }
        
		void SetColorF(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(3)) return;
            
            if (args.Assert(args[0]->IsNumber(), "Arg0 is the Red Component between 0.0f and 1.0f") ||
                args.Assert(args[1]->IsNumber(), "Arg1 is the Green Component between 0.0f and 1.0f") ||
                args.Assert(args[2]->IsNumber(), "Arg2 is the Blue Component between 0.0f and 1.0f")) return;
            
            GetDraw2D(args.This())->GetRender()->SetColor(
                args.NumberValue(0), args.NumberValue(1), args.NumberValue(2));
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
		
		void SetColor(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            _setColor(args.GetIsolate(), GetDraw2D(args.This()), args[0]);
		}
		
		void SetColorI(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(3)) return;
            
            if (args.Assert(args[0]->IsNumber(), "Arg0 is the Red Component between 0 and 255") ||
                args.Assert(args[1]->IsNumber(), "Arg1 is the Green Component between 0 and 255") ||
                args.Assert(args[2]->IsNumber(), "Arg2 is the Blue Component between 0 and 255")) return;
            
			GetDraw2D(args.This())->GetRender()->SetColor(
                args.NumberValue(0) / 255, args.NumberValue(1) / 255, args.NumberValue(2) / 255);
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
        
        void GetColor(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            args.SetReturnValue(_getColor(args.GetIsolate(), GetDraw2D(args.This())));
        }
        
        void GetRGBFromHSV(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(3)) return;
            
            if (args.Assert(args[0]->IsNumber(), "Arg0 is the Hue Component between 0 and 360") ||
                args.Assert(args[1]->IsNumber(), "Arg1 is the Saturation Component between 0.0f and 1.0f") ||
                args.Assert(args[2]->IsNumber(), "Arg2 is the Value Component between 0.0f and 1.0f")) return;

            args.SetReturnValue(Color4fToObject(args.GetIsolate(), Color4f::FromHSV(args.NumberValue(0), args.NumberValue(1), args.NumberValue(2))));
        }
        
        void ClearColor(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(1)) return;
            
            if (args[0]->IsNumber()) {
                GetDraw2D(args.This())->GetRender()->ClearColor(args.Int32Value(0));
            } else if (args[0]->IsString()) {
                GetDraw2D(args.This())->GetRender()->ClearColor(args.StringValue(0));
            } else if (args[0]->IsObject()) {
                GetDraw2D(args.This())->GetRender()->ClearColor(Color4fFromObject(args.GetIsolate(), args[0]->ToObject()));
            } else {
                args.ThrowArgError("Arg0 needs to be a string(colorName) or a number(in the format 0xrrggbb)");
            }
        }
        
        void LoadFont(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(2)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the name you use to refer to the font") ||
                args.Assert(args[1]->IsString(), "Arg1 is the filename of the font")) return;
            
            GetDraw2D(args.This())->GetRender()->LoadFont(args.StringValue(0), args.StringValue(1));
        }
        
        void SetFont(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(2)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the name of the font to use") ||
                args.Assert(args[1]->IsNumber(), "Arg1 is the size of the font to use")) return;
            
            GetDraw2D(args.This())->GetRender()->SetFont(args.StringValue(0), args.NumberValue(1));
        }
        
        void IsFontLoaded(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the name of the font to check")) return;
            
            args.SetReturnValue(args.NewBoolean(GetDraw2D(args.This())->GetRender()->IsFontLoaded(args.StringValue(0))));
        }
        
        void Print(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(3)) return;

            if (args.Assert(args[0]->IsNumber(), "Arg0 is the X position of Arg2") ||
                args.Assert(args[1]->IsNumber(), "Arg1 is the Y position of Arg2") ||
                args.Assert(args[2]->IsString(), "Arg2 is the string to render")) return;
            
            GetDraw2D(args.This())->GetRender()->Print(args.NumberValue(0), args.NumberValue(1), args.CStringValue(2));
        }
        
        void GetStringWidth(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the string to get the width of")) return;
            
            args.SetReturnValue(args.NewNumber(GetDraw2D(args.This())->GetRender()->CalcStringWidth(args.StringValue(0))));
        }
        
        // Texture Handling
        
        void Draw(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            GetDraw2D(args.This())->GetRender()->CheckError("JSDraw::Draw::PreDraw");
            
            if (args.AssertCount(5)) return;
            
            if (args.Assert(args[0]->IsExternal(), "Arg0 is a valid texture that's been loaded since the last context change") ||
                args.Assert(args[1]->IsNumber(), "Arg1 has to be X of a rect") ||
                args.Assert(args[2]->IsNumber(), "Arg2 has to be Y of a rect") ||
                args.Assert(args[3]->IsNumber(), "Arg3 has to be Width of a rect") ||
                args.Assert(args[4]->IsNumber(), "Arg4 has to be Height of a rect")) return;
            
            TexturePtr tex = (TexturePtr)args.ExternalValue(0);
            
            if (tex->IsValid()) {
                GetDraw2D(args.This())->DrawImage(tex,
                                                  args.NumberValue(1),
                                                  args.NumberValue(2),
                                                  args.NumberValue(3),
                                                  args.NumberValue(4));
            } else {
                args.ThrowArgError("Arg0 is not a valid texture");
            }
        }
        
        void DrawSub(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            GetDraw2D(args.This())->GetRender()->CheckError("JSDraw::DrawSub::PreDraw");
            
            if (args.AssertCount(9)) return;
            
            if (args.Assert(args[0]->IsExternal(), "Arg0 is a valid texture that's been loaded since the last context change") ||
                args.Assert(args[1]->IsNumber(), "Arg1 has to be X of a rect") ||
                args.Assert(args[2]->IsNumber(), "Arg2 has to be Y of a rect") ||
                args.Assert(args[3]->IsNumber(), "Arg3 has to be Width of a rect") ||
                args.Assert(args[4]->IsNumber(), "Arg4 has to be Height of a rect") ||
                args.Assert(args[5]->IsNumber(), "Arg5 has to be X of a sub rectangle") ||
                args.Assert(args[6]->IsNumber(), "Arg6 has to be Y of a sub rectangle") ||
                args.Assert(args[7]->IsNumber(), "Arg7 has to be Width of a sub rectangle") ||
                args.Assert(args[8]->IsNumber(), "Arg8 has to be Height of a sub rectangle")) return;
            
            TexturePtr tex = (TexturePtr)args.ExternalValue(0);
            
            if (tex->IsValid()) {
                GetDraw2D(args.This())->DrawImage(tex,
                                                  args.NumberValue(1),
                                                  args.NumberValue(2),
                                                  args.NumberValue(3),
                                                  args.NumberValue(4),
                                                  args.NumberValue(5),
                                                  args.NumberValue(6),
                                                  args.NumberValue(7),
                                                  args.NumberValue(8));
            } else {
                args.ThrowArgError("Arg0 is not a valid texture");
            }
        }
        
        void DrawSprite(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(6)) return;
            
            if (args.Assert(args[0]->IsExternal(), "Arg0 is a valid spritesheet that's been loaded since the last context change") ||
                args.Assert(args[1]->IsString(), "Arg1 is the sprite or animation to draw") ||
                args.Assert(args[2]->IsNumber(), "Arg1 has to be X of a rect") ||
                args.Assert(args[3]->IsNumber(), "Arg2 has to be Y of a rect") ||
                args.Assert(args[4]->IsNumber(), "Arg3 has to be Width of a rect") ||
                args.Assert(args[5]->IsNumber(), "Arg4 has to be Height of a rect")) return;
            
            SpriteSheetPtr sheet = (SpriteSheetPtr) args.ExternalValue(0);
            
            if (sheet->IsValid()) {
                GetDraw2D(args.This())->DrawSprite(sheet,
                                                   args.StringValue(1),
                                                   args.NumberValue(2),
                                                   args.NumberValue(3),
                                                   args.NumberValue(4),
                                                   args.NumberValue(5));
            } else {
                args.ThrowArgError("Arg0 is not a valid spritesheet");
            }
        }
        
        void OpenImage(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the filename of the image to load")) return;
            
            GetDraw2D(args.This())->GetRender()->CheckError("Pre Image Load");
            
            if (!Filesystem::FileExists(args.StringValue(0))) {
                args.ThrowArgError("File does not Exist");
                return;
            }
            
            std::string filename = args.StringValue(0);
            
            if (!ResourceManager::HasSource(filename)) {
                ResourceManager::Load(filename);
            }
            
            ResourceManager::ImageResourcePtr img = new ResourceManager::ImageResource(filename);
            
            img->Load();
            
            args.SetReturnValue(args.NewExternal(img->GetTexture()));
        }
        
        void OpenSpriteSheet(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the filename of the spritesheet json to load")) return;
            
            std::string filename = args.StringValue(0);
            
            if (!ResourceManager::HasSource(filename)) {
                ResourceManager::Load(filename);
            }
            
            args.SetReturnValue(args.NewExternal(SpriteSheetReader::LoadSpriteSheetFromFile(filename)));
        }
        
        void GetImageArray(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the filename to load")) return;
            
            v8::Handle<v8::Object> array = args.NewObject();
            
            long fileSize = 0;
            
            unsigned char* file = (unsigned char*) Filesystem::GetFileContent(args.StringValue(0), fileSize);
            
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
            
            arr.Reset(args.GetIsolate(), array);
            
            arr.MarkIndependent();
            arr.SetWeak(rawArray, ReadBufferWeakCallback);
            
            args.GetIsolate()->AdjustAmountOfExternalAllocatedMemory(imageSize * sizeof(float));
            
            array->SetIndexedPropertiesToExternalArrayData(rawArray, v8::kExternalFloatArray, imageSize * sizeof(float));
            
            array->Set(args.NewString("length"), args.NewNumber(imageSize));
            array->Set(args.NewString("width"), args.NewNumber(imageWidth));
            array->Set(args.NewString("height"), args.NewNumber(imageHeight));
            array->Set(args.NewString("byteLength"), args.NewNumber(static_cast<int32_t>(imageSize)));
            
            int pos = 0;
            int i = 0;
            
            for (int x = 0; x < imageWidth; x++) {
                for (int y = 0; y < imageHeight; y++) {
                    array->Set(i + 0, args.NewNumber((float) pixel[pos + 2] / 256.f));
                    array->Set(i + 1, args.NewNumber((float) pixel[pos + 1] / 256.f));
                    array->Set(i + 2, args.NewNumber((float) pixel[pos + 0] / 256.f));
                    array->Set(i + 3, args.NewNumber(1.0f));
                    pos += 4;
                    i += 4;
                }
            }
            
            SOIL_free_image_data(pixel);
            
            args.SetReturnValue(array);
        }
        
        void CreateImageArray(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(2)) return;
            
            if (args.Assert(args[0]->IsInt32(), "Arg0 is the width of the new Image Array") ||
                args.Assert(args[1]->IsInt32(), "Arg1 is the height of the new Image Array")) return;
            
            int imageWidth = args.Int32Value(0),
                imageHeight = args.Int32Value(1);
            
            int arraySize = imageWidth * imageHeight * 4;
            
            v8::Handle<v8::Object> array = args.NewObject();
            
            float* rawArray = new float[arraySize];
            
            for (int i = 0; i < arraySize; i += 4) {
                rawArray[i] = 1.0f;
                rawArray[i + 1] = 1.0f;
                rawArray[i + 2] = 1.0f;
                rawArray[i + 3] = 1.0f;
            }
            
            v8::Persistent<v8::Object> arr;
            
            arr.Reset(args.GetIsolate(), array);
            
            arr.MarkIndependent();
            arr.SetWeak(rawArray, ReadBufferWeakCallback);
            
            args.GetIsolate()->AdjustAmountOfExternalAllocatedMemory(arraySize * sizeof(float));
            
            array->SetIndexedPropertiesToExternalArrayData(rawArray, v8::kExternalFloatArray, arraySize * sizeof(float));
            
            array->Set(args.NewString("length"), args.NewNumber(arraySize));
            array->Set(args.NewString("width"), args.NewNumber(imageWidth));
            array->Set(args.NewString("height"), args.NewNumber(imageHeight));
            array->Set(args.NewString("byteLength"), args.NewNumber(arraySize * sizeof(float)));
            
            args.SetReturnValue(array);
        }
        
        void CreateImage(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.Length() < 3) {
                args.ThrowArgError("Wrong number of args draw.createImage takes 3 or 4 args");
                return;
            }
            
            if (args.Assert(args[0]->IsObject(), "Arg0 is a Object") ||
                args.Assert(args[1]->IsInt32(), "Arg1 is the width of the new image") ||
                args.Assert(args[2]->IsInt32(), "Arg2 is the height of the new image")) return;
            
            unsigned int texID = 0;
            
            if (args.Length() == 4) {
                if (args.Assert(args[3]->IsInt32(), "Arg3 is the texture ID to reuse")) return;
                texID = args.Int32Value(3);
            }
            
            v8::Handle<v8::Object> arr = v8::Handle<v8::Object>::Cast(args[0]);
            
            int width = args.Int32Value(1),
                height = args.Int32Value(2);
            
            float* pixels = NULL;
            
            if (arr->HasIndexedPropertiesInExternalArrayData()) {
                pixels = (float*) arr->GetIndexedPropertiesExternalArrayData(); // yay fast
            } else {
                int len = width * height;
                
                bool noClamp = !Config::GetBoolean("core.render.clampTexture");
                
                if (arr->Get(args.NewString("length"))->NumberValue() != width * height * 4) {
                    args.ThrowArgError("Array size != width * height * 3");
                    return;
                }
                
                pixels = new float[len * 4];
                
                int i2 = 0;
                
                for (int i = 0; i < len * 4; i += 4) {
                    if (noClamp) {
                        pixels[i + 0] = (float) arr->Get(i2 + 0)->NumberValue();
                        pixels[i + 1] = (float) arr->Get(i2 + 1)->NumberValue();
                        pixels[i + 2] = (float) arr->Get(i2 + 2)->NumberValue();
                        pixels[i + 3] = (float) arr->Get(i2 + 4)->NumberValue();
                    } else {
                        pixels[i + 0] = glm::clamp((float) arr->Get(i2 + 0)->NumberValue(), 0.0f, 1.0f);
                        pixels[i + 1] = glm::clamp((float) arr->Get(i2 + 1)->NumberValue(), 0.0f, 1.0f);
                        pixels[i + 2] = glm::clamp((float) arr->Get(i2 + 2)->NumberValue(), 0.0f, 1.0f);
                        pixels[i + 3] = glm::clamp((float) arr->Get(i2 + 3)->NumberValue(), 0.0f, 1.0f);
                    }
                    i2 += 3;
                }
            }
            
            TexturePtr t = ImageReader::TextureFromBuffer(texID, pixels, width, height);
            
            if (!arr->HasIndexedPropertiesInExternalArrayData()) {
                delete [] pixels;
            }
            
            args.SetReturnValue(args.NewExternal(t));
        }
        
        void SaveImage(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(2)) return;
            
            if (args.Assert(args[0]->IsExternal(), "Arg0 is the index of the image returned from draw.createImage or draw.openImage") ||
                args.Assert(args[1]->IsString(), "Arg1 is the filename to save the image as")) return;
            
            TexturePtr tex = (TexturePtr)args.ExternalValue(0);
            
            if (tex->IsValid()) {
                tex->Save(args.StringValue(1));
                
                Logger::begin("SaveImage", Logger::LogLevel_Log)
                << "Saved image as: " << Filesystem::GetRealPath(args.StringValue(1))
                << Logger::end();
            } else {
                args.ThrowArgError("Arg0 is not a valid textureID");
            }
        }
        
        void FreeImage(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsExternal(), "Arg0 is the texture to free")) return;
            
            ((TexturePtr) args.ExternalValue(0))->Invalidate();
        }
        
        void IsTexture(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(1)) return;
            
            if (args[0]->IsExternal()) {
                args.SetReturnValue(args.NewBoolean(((TexturePtr) args.ExternalValue(0))->IsValid()));
            } else {
                args.SetReturnValue(args.NewBoolean(0));
            }
        }
        
        void IsSpriteSheet(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(1)) return;
            
            if (args[0]->IsExternal()) {
                args.SetReturnValue(args.NewBoolean(((SpriteSheetPtr) args.ExternalValue(0))->IsValid()));
            } else {
                args.SetReturnValue(args.NewBoolean(0));
            }
        }
        
        // basicly restarts 2d drawing
        void CameraReset(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            GetDraw2D(args.This())->GetRender()->Reset();
        }
        
        void CameraPan(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(2)) return;
            
            if (args.Assert(args[0]->IsNumber(), "Arg0 is the X Distince to pan") ||
                args.Assert(args[1]->IsNumber(), "Arg1 is the Y Distince to pan")) return;
            
            GetDraw2D(args.This())->GetRender()->CameraPan(args.NumberValue(0), args.NumberValue(1));
        }
        
        void CameraZoom(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsNumber(), "Arg0 is the factor to zoom the camera")) return;
            
            GetDraw2D(args.This())->GetRender()->CameraZoom(args.NumberValue(0));
        }
        
        void CameraRotate(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(HasGLContext(), "No OpenGL Context")) return;
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsNumber(), "Arg0 is the factor to rotate the camera by")) return;
            
            GetDraw2D(args.This())->GetRender()->CameraRotate(args.NumberValue(0));
        }
        
        void SetCenter(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(2)) return;
            
            if (args.Assert(args[0]->IsInt32(), "Arg0 is the x to offset drawing by") ||
                args.Assert(args[1]->IsInt32(), "Arg1 is the y to offset drawing by")) return;
            
            GetDraw2D(args.This())->GetRender()->SetCenter(args.Int32Value(0), args.Int32Value(1));
        }
        
        void DrawColorGetter(v8::Local<v8::String> prop, const v8::PropertyCallbackInfo<v8::Value>& info) {
            info.GetReturnValue().Set(_getColor(info.GetIsolate(), GetDraw2D(info.Holder())));
        }
        
        void DrawColorSetter(v8::Local<v8::String> prop, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
            _setColor(info.GetIsolate(), GetDraw2D(info.Holder()), value);
        }
        
        void InitDraw(v8::Handle<v8::ObjectTemplate> drawTable) {
            InitColorObject(drawTable);
            JS_VertexBuffer2D::Init(drawTable);
            
            ScriptingManager::Factory f(v8::Isolate::GetCurrent());
            
            f.FillTemplate(drawTable, {
                {FTT_Static, "rect", f.NewFunctionTemplate(Rect)},
                {FTT_Static, "grid", f.NewFunctionTemplate(Grid)},
                {FTT_Static, "grad", f.NewFunctionTemplate(Grad)},
                {FTT_Static, "circle", f.NewFunctionTemplate(Circle)},
                {FTT_Static, "curve", f.NewFunctionTemplate(Curve)},
                {FTT_Static, "line", f.NewFunctionTemplate(Rect)},
                {FTT_Static, "polygon", f.NewFunctionTemplate(Polygon)},
                
                {FTT_Static, "colorPalette", f.NewFunctionTemplate(ColorPalette)},
                {FTT_Static, "setColorF", f.NewFunctionTemplate(SetColorF)},
                {FTT_Static, "setColor", f.NewFunctionTemplate(SetColor)},
                {FTT_Static, "setColorI", f.NewFunctionTemplate(SetColorI)},
                {FTT_Static, "getColor", f.NewFunctionTemplate(GetColor)},
                {FTT_Static, "clearColor", f.NewFunctionTemplate(ClearColor)},
                {FTT_Static, "getRGBFromHSV", f.NewFunctionTemplate(GetRGBFromHSV)},
                
                {FTT_Static, "print", f.NewFunctionTemplate(Print)},
                {FTT_Static, "getStringWidth", f.NewFunctionTemplate(GetStringWidth)},
                {FTT_Static, "draw", f.NewFunctionTemplate(Draw)},
                {FTT_Static, "drawSub", f.NewFunctionTemplate(DrawSub)},
                {FTT_Static, "drawSprite", f.NewFunctionTemplate(DrawSprite)},
                {FTT_Static, "openImage", f.NewFunctionTemplate(OpenImage)},
                {FTT_Static, "openSpriteSheet", f.NewFunctionTemplate(OpenSpriteSheet)},
                {FTT_Static, "getImageArray", f.NewFunctionTemplate(GetImageArray)},
                {FTT_Static, "createImageArray", f.NewFunctionTemplate(CreateImageArray)},
                {FTT_Static, "createImage", f.NewFunctionTemplate(CreateImage)},
                {FTT_Static, "saveImage", f.NewFunctionTemplate(SaveImage)},
                {FTT_Static, "freeImage", f.NewFunctionTemplate(FreeImage)},
                {FTT_Static, "isTexture", f.NewFunctionTemplate(IsTexture)},
                {FTT_Static, "isSpriteSheet", f.NewFunctionTemplate(IsSpriteSheet)},
                
                {FTT_Static, "cameraReset", f.NewFunctionTemplate(CameraReset)},
                {FTT_Static, "cameraPan", f.NewFunctionTemplate(CameraPan)},
                {FTT_Static, "cameraZoom", f.NewFunctionTemplate(CameraZoom)},
                {FTT_Static, "cameraRotate", f.NewFunctionTemplate(CameraRotate)},
                
                {FTT_Static, "setFont", f.NewFunctionTemplate(SetFont)},
                {FTT_Static, "loadFont", f.NewFunctionTemplate(LoadFont)},
                {FTT_Static, "isFontLoaded", f.NewFunctionTemplate(IsFontLoaded)},
                
                {FTT_Static, "setCenter", f.NewFunctionTemplate(SetCenter)},
            });
            
            drawTable->SetAccessor(f.NewString("drawColor"), DrawColorGetter, DrawColorSetter);
        }
        
    } // namespace JsDraw
} // namespace Engine