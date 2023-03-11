#include <rendevox-base.hpp>
#include <openglRender/openglRender.hpp>

#include <vector>

std::vector<float> verticesBuffer = {};

OpenglRender::OpenglRender(OpenglWindow &openglWindow) {
	this->openglWindow = &openglWindow;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}

OpenglRender::~OpenglRender() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Rendevox::DrawTriangle(Vector2 pos1, Vector2 pos2, Vector2 pos3, Vector3 pos1color, Vector3 pos2color,
                            Vector3 pos3color) {
    verticesBuffer.push_back(pos1.x);
    verticesBuffer.push_back(pos1.y);
    verticesBuffer.push_back(0.0f);

    verticesBuffer.push_back(pos1color.x);
    verticesBuffer.push_back(pos1color.y);
    verticesBuffer.push_back(pos1color.z);

    verticesBuffer.push_back(pos2.x);
    verticesBuffer.push_back(pos2.y);
    verticesBuffer.push_back(0.0f);

    verticesBuffer.push_back(pos2color.x);
    verticesBuffer.push_back(pos2color.y);
    verticesBuffer.push_back(pos2color.z);

    verticesBuffer.push_back(pos3.x);
    verticesBuffer.push_back(pos3.y);
    verticesBuffer.push_back(0.0f);

    verticesBuffer.push_back(pos3color.x);
    verticesBuffer.push_back(pos3color.y);
    verticesBuffer.push_back(pos3color.z);
}

void OpenglRender::Draw() {
    UserUpdate();

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (GLint)verticesBuffer.size() * 4, verticesBuffer.data(), GL_DYNAMIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // render the triangle
    glDrawArrays(GL_TRIANGLES, 0, verticesBuffer.size() / 3);

    verticesBuffer.clear();
}
