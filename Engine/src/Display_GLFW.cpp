#include "Display.hpp"

namespace Engine {
	namespace Display {
		void Init(int width, int height) {
			if (!glfwInit()) {
				Logger::WriteError("Could not init GLFW");
			}
			if (!glfwOpenWindow(width, height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)) {
				Logger::WriteError("Could not init GLFW");
			}
			glewExperimental = GL_TRUE;
			glewInit();
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