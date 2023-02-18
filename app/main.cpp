#include "../rendevox-base/rendevox-base.hpp"

int main() {
	Rendevox::Window window = Rendevox::Window("OpenGL", 1280, 720, "RVX", false);

	return 0;
}

void UserUpdate() {
    Rendevox::DrawTriangle(Vector2(-0.5f, -0.5f),
                 Vector2(-0.5f, 0.5f),
                 Vector2(0.5f, 0.5f),
                 Vector3(1.0f, 0.0f, 0.0f),
                 Vector3(0.0f, 1.0f, 0.0f),
                 Vector3(0.0f, 0.0f, 1.0f));
    Rendevox::DrawTriangle(Vector2(0.5f, -0.5f),
                           Vector2(-0.5f, -0.5f),
                           Vector2(0.5f, 0.5f),
                           Vector3(0.0f, 1.0f, 0.0f),
                           Vector3(1.0f, 0.0f, 0.0f),
                           Vector3(0.0f, 0.0f, 1.0f));
}
