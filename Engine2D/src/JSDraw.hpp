#ifndef __JSDRAWH
#define __JSDRAWH

#include "common.hpp"
#include "main.hpp"

void Begin2d();
void End2d();

v8::Handle<v8::Value> JS_Draw_Drawfunc(const v8::Arguments& args);
v8::Handle<v8::Value> JS_Draw_Begin(const v8::Arguments& args);
v8::Handle<v8::Value> JS_Draw_End(const v8::Arguments& args);
v8::Handle<v8::Value> JS_Draw_Rect(const v8::Arguments& args);
v8::Handle<v8::Value> JS_Draw_Grid(const v8::Arguments& args);
v8::Handle<v8::Value> JS_Draw_Grad(const v8::Arguments& args);
v8::Handle<v8::Value> JS_Draw_Draw(const v8::Arguments& args);
v8::Handle<v8::Value> JS_Draw_SetColorF(const v8::Arguments& args);
v8::Handle<v8::Value> JS_Draw_SetColor(const v8::Arguments& args);
v8::Handle<v8::Value> JS_Draw_SetColorI(const v8::Arguments& args);
v8::Handle<v8::Value> JS_Draw_ClearColor(const v8::Arguments& args);
v8::Handle<v8::Value> JS_Draw_Print(const v8::Arguments& args);
v8::Handle<v8::Value> JS_Draw_OpenImage(const v8::Arguments& args);

#endif