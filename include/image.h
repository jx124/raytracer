#pragma once

#include "glad/glad.h"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>

class Image {
public:
    Image(int width, int height);
    
    void init();
    void display();
    inline std::vector<float>& getBuffer() { return buffer; };
    
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