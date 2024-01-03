#pragma once

#include "glad/glad.h"
#include "glm/vec3.hpp"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>

class Image {
public:
    Image(int width, int height);
    ~Image();

    void init();
    void display();
    void writePixel(int i, int j, const glm::vec3& value);
    
private:
    void compileShaders(std::string vertexPath, std::string fragmentPath);

    GLuint shaderID;
    GLuint imageTexture;
    GLuint VAO;
    GLuint VBO;

    int width;
    int height;
    std::vector<float> buffer;
};