#include "../rendevox.hpp"

Window::Window(const char* _renderType, int _width, int _height, const char* _title, bool _fullscreen) {
	renderType = _renderType;
	width = _width;
	height = _height;

	title = _title;
	fullscreen = _fullscreen;
	
	if (strcmp(renderType, "OpenGL") == 0) {
		OpenglWindow openglWindow = OpenglWindow(this);
	}
}
