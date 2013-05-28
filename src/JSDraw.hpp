#pragma once

#include "common.hpp"
#include "main.hpp"

#include "Filesystem.hpp"

namespace Engine {

	namespace JsDraw {

		void Begin2d();
		void End2d();
        
        ENGINE_JS_METHOD(SetShader);

		ENGINE_JS_METHOD(Rect);
		ENGINE_JS_METHOD(Grid);
		ENGINE_JS_METHOD(Grad);
		ENGINE_JS_METHOD(SetColorF);
		ENGINE_JS_METHOD(SetColor);
		ENGINE_JS_METHOD(SetColorI);
		ENGINE_JS_METHOD(ClearColor);
		ENGINE_JS_METHOD(Print);
        
        ENGINE_JS_METHOD(Draw);
        ENGINE_JS_METHOD(DrawSub);
		ENGINE_JS_METHOD(OpenImage);
        ENGINE_JS_METHOD(CreateImage);
        ENGINE_JS_METHOD(FreeImage);
        
        ENGINE_JS_METHOD(CameraReset);
        ENGINE_JS_METHOD(CameraPan);
        ENGINE_JS_METHOD(CameraZoom);
        ENGINE_JS_METHOD(CameraRotate);
        
        ENGINE_JS_METHOD(GetTextWidth);
        
        ENGINE_JS_METHOD(GetVerts);
        ENGINE_JS_METHOD(SetDrawOffscreen);
        ENGINE_JS_METHOD(SetCenter);
	
	}

}