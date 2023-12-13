#pragma once

#include "image.h"
#include "utils.h"
#include "hittable.h"
#include "scene.h"
#include "sphere.h"
#include "camera.h"

class Renderer {
public:
    Renderer(int width, int height);
    void onRender();

private:
    int width, height;
    Image image;
    Camera camera;
    Scene scene;
};