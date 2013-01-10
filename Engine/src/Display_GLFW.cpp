#include "Display.hpp"

namespace Engine {
	namespace Display {
		void Init(int width, int height) {
			glfwInit();
			glfwOpenWindow(width, height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW);
			glViewport(0, 0, width, height);
		}

		void Present() {
			glfwSwapBuffers();
		}

		bool IsOpen() {
			return glfwGetWindowParam(GLFW_OPENED);
		}

		bool IsKeyDown() {
			return false;
		}

		void SetTitle(std::string str) {
			glfwSetWindowTitle(str.c_str());
		}
	}
}