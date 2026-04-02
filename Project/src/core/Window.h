#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
    GLFWwindow* handle;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    Window(int width, int height, const std::string& title);
    ~Window();

    bool shouldClose();
    void updateTime();
    void clear(float r, float g, float b);
    void swapBuffers();
};