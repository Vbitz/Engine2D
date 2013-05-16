#pragma once

#include "common.hpp"
#include "main.hpp"

#include "Filesystem.hpp"

namespace Engine {

	namespace JsDraw {

		void						Begin2d();
		void						End2d();

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
        
        v8::Handle<v8::Value>       CameraReset(const v8::Arguments& args);
        v8::Handle<v8::Value>       CameraPan(const v8::Arguments& args);
        v8::Handle<v8::Value>       CameraZoom(const v8::Arguments& args);
        v8::Handle<v8::Value>       CameraRotate(const v8::Arguments& args);
        
        v8::Handle<v8::Value>       GetTextWidth(const v8::Arguments& args);
        
        v8::Handle<v8::Value>       GetVerts(const v8::Arguments& args);
        v8::Handle<v8::Value>       SetDrawOffscreen(const v8::Arguments& args);
        v8::Handle<v8::Value>       SetCenter(const v8::Arguments& args);
	
	}

}