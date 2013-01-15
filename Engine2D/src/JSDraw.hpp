#pragma once

#include "common.hpp"
#include "main.hpp"

#include "Filesystem.hpp"

namespace Engine {

	namespace JsDraw {

		void						Begin2d();
		void						End2d();

		v8::Handle<v8::Value>		Drawfunc(const v8::Arguments& args);
		v8::Handle<v8::Value>		Begin(const v8::Arguments& args);
		v8::Handle<v8::Value>		End(const v8::Arguments& args);
		v8::Handle<v8::Value>		Rect(const v8::Arguments& args);
		v8::Handle<v8::Value>		Grid(const v8::Arguments& args);
		v8::Handle<v8::Value>		Grad(const v8::Arguments& args);
		v8::Handle<v8::Value>		Draw(const v8::Arguments& args);
		v8::Handle<v8::Value>		SetColorF(const v8::Arguments& args);
		v8::Handle<v8::Value>		SetColor(const v8::Arguments& args);
		v8::Handle<v8::Value>		SetColorI(const v8::Arguments& args);
		v8::Handle<v8::Value>		ClearColor(const v8::Arguments& args);
		v8::Handle<v8::Value>		Print(const v8::Arguments& args);
		v8::Handle<v8::Value>		OpenImage(const v8::Arguments& args);
	
	}

}