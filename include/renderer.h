#pragma once

#include "image.h"
#include "utils.h"
#include "hittable.h"
#include "scene.h"
#include "sphere.h"
#include "quad.h"
#include "camera.h"
#include "sampler.h"
#include "bvh_node.h"

#include "omp.h"

#include <chrono>

class Renderer {
public:
    Renderer(int width, int height);
    void onRender();
    void displayImage() { image.display(); };
    Scene generateScene(int index);

private:
    int width, height;
    int maxDepth = 50;
    bool complete = false;
    Image image;
    Camera camera;
    Scene scene;
    std::unique_ptr<Sampler> sampler;
};