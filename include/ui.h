#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "renderer.h"

#include <string>
#include <iostream>
#include <optional>
#include <future>

constexpr int VERSION_MAJOR = 4;
constexpr int VERSION_MINOR = 6;

class UI {
public:
    UI(int width, int height, std::string title);
    ~UI();

    void run();

private:
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                              GLsizei length, const GLchar* message, const void* param);
    void processInput(GLFWwindow* window);

    GLFWwindow* window;
    std::optional<Renderer> renderer; // Needs an optional to defer construction to after GLFW is initialized.
};