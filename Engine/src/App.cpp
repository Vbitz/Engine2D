#include "App.hpp"

namespace Engine {
	App::App() {
		this->_running = false;
	}

	int App::Run() {
		this->_running = true;

		Display::Init(800, 600);
		Display::SetTitle("Engine");

		Render::SetClearColor(100, 149, 237);

		this->_mainLoop();
		return 0;
	}

	void App::Draw() {

	}

	void App::Update() {

	}

	void App::Resize(int width, int height) {
		Render::ResizeViewpoint(width, height);
	}

	void App::Stop() {
		this->_running = false;
	}

	void App::_mainLoop() {
		while (this->_running) {
			Render::ClearDisplay();

			this->Update();

			this->Draw();

			Display::Present();

			if (!Display::IsOpen()) {
				this->Stop();
			}
		}
	}
}