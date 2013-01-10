#include "App.hpp"

namespace Engine {
	App::App(const char* argv0) {
		this->_running = false;
		Filesystem::Init(argv0);
	}

	int App::Run() {
		this->_running = true;

		Display::Init(800, 600);
		Display::SetTitle("Engine");

		Render::SetClearColor(100, 149, 237);

		_testingModel.SetShader(Render::Shader(Render::PreDefinedShader_Basic));
		_testingModel.SetMode(Render::RenderMode_Triangles);
		_testingModel.AddVertex(1, 1, 1);
		_testingModel.AddVertex(0, 1, 1);
		_testingModel.AddVertex(1, 0, 1);
		
		_testingModel.Upload();

		this->_mainLoop();

		this->Finish();

		return 0;
	}

	void App::Draw() {
		_testingModel.Draw();
	}

	void App::Update() {

	}

	void App::Resize(int width, int height) {
		Render::ResizeViewpoint(width, height);
	}

	void App::Stop() {
		this->_running = false;
	}

	void App::Finish() {
		Filesystem::Destroy();
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