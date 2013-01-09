#pragma once

#include "OpenGL.hpp"

namespace Engine {
	namespace Render {
		void ResizeViewpoint(int w, int h);

		void SetClearColor(int r, int g, int b);
		void ClearDisplay();
	}
}