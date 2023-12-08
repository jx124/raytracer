#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <string>
#include <iostream>

constexpr int VERSION_MAJOR = 4;
constexpr int VERSION_MINOR = 6;

class UI {
public:
    UI(int width, int height, std::string title) {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, VERSION_MAJOR);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, VERSION_MINOR);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (window == nullptr) {
            std::cout << "Failed to create GLFW window" << std::endl;
            return;
        }

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }

        glDebugMessageCallback(debugCallback, nullptr);
        std::printf("OpenGL Version: %s\nOpenGL Renderer: %s\n", glGetString(GL_VERSION), glGetString(GL_RENDERER));
    }

    ~UI() {
        glfwTerminate();
    }

    void run() {
        while (!glfwWindowShouldClose(window)) {
            processInput(window);

            // render loop
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glfwSwapBuffers(window);
            glfwPollEvents();            
        }
    }

private:
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* param);
    void processInput(GLFWwindow* window);

    GLFWwindow* window;
};