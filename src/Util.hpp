/*
   Filename: Util.hpp
   Purpose:  Utility functions

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

#pragma once

#include <include/v8.h>
#include <string>

#ifdef _WIN32
#define sleep Sleep
#else
#include <unistd.h>
#endif

#define ENGINE_THROW_ARGCOUNT(count) v8::Isolate::GetCurrent()->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "Wrong number of arguments, expected " + count)));
#define ENGINE_THROW_ARGERROR(str) v8::Isolate::GetCurrent()->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), str)));

#define ENGINE_JS_METHOD(name) void name(const v8::FunctionCallbackInfo<v8::Value>& args)

#define ENGINE_CHECK_ARGS_LENGTH(num) if (args.Length() != num) {ENGINE_THROW_ARGCOUNT(num);ENGINE_JS_SCOPE_CLOSE_UNDEFINED;}

#define ENGINE_CHECK_ARG_INT32(num, errStr) if (!args[num]->IsNumber()) {ENGINE_THROW_ARGERROR(errStr);ENGINE_JS_SCOPE_CLOSE_UNDEFINED;}
#define ENGINE_CHECK_ARG_NUMBER(num, errStr) ENGINE_CHECK_ARG_INT32(num, errStr)
#define ENGINE_GET_ARG_INT32_VALUE(num) args[num]->Int32Value()
#define ENGINE_GET_ARG_NUMBER_VALUE(num) args[num]->NumberValue()

#define ENGINE_CHECK_ARG_FUNCTION(num, errStr) if (!args[num]->IsFunction()) {ENGINE_THROW_ARGERROR(errStr);ENGINE_JS_SCOPE_CLOSE_UNDEFINED;}

#define ENGINE_CHECK_ARG_STRING(num, errStr) if (!args[num]->IsString()) {ENGINE_THROW_ARGERROR(errStr);ENGINE_JS_SCOPE_CLOSE_UNDEFINED;}
#define ENGINE_GET_ARG_CSTRING_VALUE(num) v8::String::Utf8Value(args[num])
#define ENGINE_GET_ARG_CPPSTRING_VALUE(num) std::string(*v8::String::Utf8Value(args[num]))

#define ENGINE_CHECK_ARG_BOOLEAN(num, errStr) if (!args[num]->IsBoolean()) {ENGINE_THROW_ARGERROR(errStr);ENGINE_JS_SCOPE_CLOSE_UNDEFINED;}
#define ENGINE_GET_ARG_BOOLEAN_VALUE(num) args[num]->ToBoolean()->Value()

#define ENGINE_CHECK_ARG_ARRAY(num, errStr) if (!args[num]->IsArray()) {ENGINE_THROW_ARGERROR(errStr);ENGINE_JS_SCOPE_CLOSE_UNDEFINED;}
#define ENGINE_GET_ARG_ARRAY(num) args[num].As<v8::Array>()

#define ENGINE_CHECK_ARG_OBJECT(num, errStr) if (!args[num]->IsObject()) {ENGINE_THROW_ARGERROR(errStr);ENGINE_JS_SCOPE_CLOSE_UNDEFINED;}
#define ENGINE_GET_ARG_OBJECT(num) args[num].As<v8::Object>()

#define ENGINE_CHECK_ARG_EXTERNAL(num, errStr) if (!args[num]->IsExternal()) {ENGINE_THROW_ARGERROR(errStr);ENGINE_JS_SCOPE_CLOSE_UNDEFINED;}
#define ENGINE_GET_ARG_EXTERNAL_VALUE(num) v8::Handle<v8::External>::Cast(args[num])->Value()

#define ENGINE_JS_SCOPE_OPEN v8::HandleScope scope(args.GetIsolate())
#define ENGINE_JS_SCOPE_CLOSE(value) args.GetReturnValue().Set(value); return
#define ENGINE_JS_SCOPE_CLOSE_UNDEFINED return

#define ENGINE_CHECK_GL if (!Engine::HasGLContext()) { ENGINE_THROW_ARGERROR("No OpenGL Context"); ENGINE_JS_SCOPE_CLOSE_UNDEFINED; }