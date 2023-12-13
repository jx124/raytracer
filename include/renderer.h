#pragma once

#include "image.h"
#include "ray.h"
#include "glm/glm.hpp"

#include <algorithm>
#include <cmath>

class Renderer {
public:
    Renderer(int width, int height);
    void onRender();

private:
    int width, height;
    Image image;
};