#include "renderer.h"

Renderer::Renderer(int width, int height) : width(width), height(height), image(width, height) {
    image.init();
}

void Renderer::onRender() {
    // render loop
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    auto &buffer = image.getBuffer();
    std::for_each(buffer.begin(), buffer.end(), [](float &f){ f += 0.01f; f = fmod(f, 1.0f); });
    image.display();
}
