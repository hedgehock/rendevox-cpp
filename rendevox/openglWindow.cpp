#include "openglWindow.hpp"

OpenglWindow::OpenglWindow(Window* _window) {
	window = _window;
    
    vertexShaderSource = "#version 330 core\n"
                          "layout (location = 0) in vec3 aPos;\n"
                          "void main()\n"
                          "{\n"
                          "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                          "}\0";

    fragmentShaderSource = "#version 330 core\n"
                            "out vec4 FragColor;\n"
                            "uniform vec4 vertexColor;\n"
                            "void main()\n"
                            "{\n"
                            "   FragColor = vertexColor;\n"
                            "}\n\0";

    Create();
    CompileShaders();
    Loop();
}

void OpenglWindow::Loop() {
    while (!glfwWindowShouldClose(glfwWindow)) {
        // Render Start
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        int vertexColorLocation = glGetUniformLocation(shaderProgram, "vertexColor");
        glUniform4f(vertexColorLocation, 1.0f, 1.0f, 1.0f, 1.0f);

        // Render End
        glfwSwapBuffers(glfwWindow);
        glfwPollEvents();
    }
}

void OpenglWindow::CompileShaders() {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Vertex shader compilation failed");
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Fragment shader compilation failed");
    }
    // link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Shader program linking failed");
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}

void OpenglWindow::Create() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, false);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindow = glfwCreateWindow(window->width, window->height, window->title, NULL, NULL);
    if (glfwWindow == NULL)
    {
        printf("Cannot create GLFW window");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(glfwWindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Cannot load GLAD");
        exit(EXIT_FAILURE);
    }
}
