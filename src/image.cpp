#include "image.h"

Image::Image(int width, int height) : width(width), height(height) {
    buffer = std::vector<float>(width * height * 3, 0.0f);
}

Image::~Image() {
    glDeleteTextures(1, &imageTexture);
    glDeleteProgram(shaderID);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);
}

void Image::init() {
    std::vector<float> vertices = {
        // vertices       // texture coords
        -1.0f,  -1.0f,    0.0f, 1.0f,
         1.0f,  -1.0f,    1.0f, 1.0f,
         1.0f,   1.0f,    1.0f, 0.0f,
        -1.0f,   1.0f,    0.0f, 0.0f,
    };
    
    // Create vertex array and vertex buffer objects
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Compile shaders and set texture parameters
    compileShaders("assets/image_vertex.glsl", "assets/image_fragment.glsl");
    glUseProgram(shaderID);
    glUniform1i(glGetUniformLocation(shaderID, "ImageTexture"), 0);

    glPixelStoref(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &imageTexture);
    glBindTexture(GL_TEXTURE_2D, imageTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Bind and set active texture
    glBindTexture(GL_TEXTURE_2D, imageTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, buffer.data());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, imageTexture);
}

void Image::display() {
    glBindTexture(GL_TEXTURE_2D, imageTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, buffer.data());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, imageTexture);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Image::writePixel(int i, int j, const glm::vec3 &value) { 
    buffer[(i + j * width) * 3 + 0] = value.r;
    buffer[(i + j * width) * 3 + 1] = value.g;
    buffer[(i + j * width) * 3 + 2] = value.b;
};

void Image::compileShaders(std::string vertexPath, std::string fragmentPath) {
    std::ostringstream vStream, fStream;
    std::ifstream vFile(vertexPath);
    std::ifstream fFile(fragmentPath);

    if (!vFile) {
        std::cerr << std::strerror(errno) << ": " << vertexPath << std::endl;
        throw std::runtime_error("Failed to open vertex shader file");
    }

    if (!fFile) {
        std::cerr << std::strerror(errno) << ": " << fragmentPath << std::endl;
        throw std::runtime_error("Failed to open fragment shader file");
    }

    // Convert file contents to strings
    vStream << vFile.rdbuf();
    fStream << fFile.rdbuf();

    const std::string vStr(vStream.str());
    const std::string fStr(fStream.str());

    const char *vertexShaderSource = vStr.c_str();
    const char *fragmentShaderSource = fStr.c_str();

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Compile shaders into program
    this->shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexShader);
    glAttachShader(shaderID, fragmentShader);
    glLinkProgram(shaderID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
