#include "Render.hpp"

namespace Engine {
	namespace Render {
		void ResizeViewpoint(int w, int h) {
			glViewport(0, 0, w, h);
		}

		void SetClearColor(int r, int g, int b) {
			glClearColor(r / 255, g / 255, b / 255, 1.0f);
		}

		void ClearDisplay() {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}
}