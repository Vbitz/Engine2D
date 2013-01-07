#pragma once

#include <gl/glfw.h>

namespace Engine {
	namespace Render {
		void ResizeViewpoint(int w, int h);

		void SetClearColor(int r, int g, int b);
		void ClearDisplay();
	}
}