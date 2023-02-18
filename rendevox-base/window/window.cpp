#include "../rendevox-base.hpp"

Window::Window(const char* renderType, int width, int height, const char* title, bool fullscreen) {
	this->renderType = renderType;
	this->width = width;
	this->height = height;
	this->title = title;
	this->fullscreen = fullscreen;
	
	if (strcmp(renderType, "OpenGL") == 0) {
		OpenglWindow openglWindow = OpenglWindow(*this);
	}
}
