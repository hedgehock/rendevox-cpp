#ifndef OPENGLRENDER_HPP
#define OPENGLRENDER_HPP

#include "../vector2.hpp"
#include "../vector3.hpp"
#include "../openglWindow/openglWindow.hpp"

struct OpenglRender {
public:
	OpenglWindow *openglWindow;
	unsigned int VBO;
	unsigned int VAO;
	float lastTime = 0.0f;

	OpenglRender(OpenglWindow &openglWindow);
    ~OpenglRender();
    void Draw();
};

namespace Rendevox {
    void DrawTriangle(Vector2 pos1, Vector2 pos2, Vector2 pos3, Vector3 pos1color, Vector3 pos2color, Vector3 pos3color);
}

#endif
