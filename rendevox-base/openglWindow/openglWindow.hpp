#ifndef OPENGLWINDOW_HPP
#define OPENGLWINDOW_HPP

#include <glad/glad.hpp>
#include <GLFW/glfw3.h>

#include <window/window.hpp>

class OpenglWindow {
public:
	Rendevox::Window* window;
	GLFWwindow* glfwWindow;
	const char* vertexShaderSource;
	const char* fragmentShaderSource;
	unsigned int shaderProgram;

    OpenglWindow(Rendevox::Window& window);
    ~OpenglWindow();
	void Create();
	void CompileShaders();
	void Loop();
};

#endif
