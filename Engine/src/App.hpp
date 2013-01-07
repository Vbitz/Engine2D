#pragma once

namespace Engine {
	class App {
	public:
		App();

		int Run();

	protected:
		void Draw();
		void Update();

	private:
		void _mainLoop();
	};
}