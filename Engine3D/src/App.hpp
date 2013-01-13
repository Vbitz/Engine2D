#pragma once

#include "Display.hpp"
#include "Render.hpp"
#include "RenderModel.hpp"
#include "Filesystem.hpp"
#include "Logger.hpp"

namespace Engine {
	class App {
	public:
		App(const char* argv0);

		int Run();

	protected:
		void Draw();
		void Update();

		void Resize(int width, int height);

		void Stop();
		void Finish();
	private:
		Render::RenderModel _testingModel;

		bool _running;

		void _mainLoop();
	};
}