#ifndef OPENGLWINDOW_HPP
#define OPENGLWINDOW_HPP

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "window.hpp"

class OpenglWindow {
private:
	Window *window;

	GLFWwindow *glfwWindow;
	const char* vertexShaderSource;
	const char* fragmentShaderSource;

	unsigned int shaderProgram;

	void Create();
	void CompileShaders();
	void Loop();
public:
	OpenglWindow(Window *_window);
};

#endif
