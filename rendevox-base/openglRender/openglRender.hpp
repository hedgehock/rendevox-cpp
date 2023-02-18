#ifndef OPENGLRENDER_HPP
#define OPENGLRENDER_HPP

#include <list>

#include "../openglWindow/openglWindow.hpp"

struct OpenglRender {
private:
	OpenglWindow &openglWindow;

	std::list<float> verticesBuffer;

	unsigned int VBO;
	unsigned int VAO;

	float lastTime = 0.0f;
public:
	OpenglRender(OpenglWindow &_openglWindow);
};

#endif
