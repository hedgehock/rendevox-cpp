#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <iostream>

class Window {
public:
	const char* renderType;
	int width;
	int height;
	
	const char* title;
	bool fullscreen;
	Window(const char* renderType, int _width, int _height, const char* _title, bool _fullscreen);
};

#endif
