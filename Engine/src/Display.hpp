#pragma once

#include <string>

#include "Render.hpp"

namespace Engine {
	namespace Display {
		void Init(int width, int height);
		void Present();

		bool IsOpen();

		bool IsKeyDown();

		void SetTitle(std::string str);
	}
}