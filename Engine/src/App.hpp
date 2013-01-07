#pragma once

#include "Display.hpp"
#include "Render.hpp"

namespace Engine {
	class App {
	public:
		App();

		int Run();

	protected:
		void Draw();
		void Update();

		void Resize(int width, int height);

		void Stop();
	private:
		bool _running;

		void _mainLoop();
	};
}