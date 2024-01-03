#pragma once

#include "hittables/hittable.h"
#include "hittables/scene.h"
#include "hittables/sphere.h"
#include "hittables/quad.h"
#include "hittables/bvh_node.h"
#include "math/sampler.h"
#include "image.h"
#include "utils.h"
#include "camera.h"
#include "integrator.h"

#include "omp.h"

#include <chrono>

class Renderer {
public:
    Renderer(int width, int height);
    void onRender();
    void displayImage();

private:
    int width, height;
    int maxDepth = 50;
    bool complete = false;
    Image image;
    Camera camera;
    Scene scene;
    std::unique_ptr<Sampler> sampler;
    std::unique_ptr<Integrator> integrator;
};