#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "image.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <cmath>

constexpr int VERSION_MAJOR = 4;
constexpr int VERSION_MINOR = 6;

class UI {
public:
    UI(int width, int height, std::string title);

    ~UI() {
        glfwTerminate();
    }

    void run();

private:
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                              GLsizei length, const GLchar* message, const void* param);
    void processInput(GLFWwindow* window);

    GLFWwindow* window;
    Image image;
};