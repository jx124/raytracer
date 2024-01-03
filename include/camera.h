#pragma once

#include "utils.h"
#include "sampler.h"

class Camera {
public:
    Camera(int imageWidth, int imageHeight);

    void init();
    Ray generateRay(int i, int j, Sampler* sampler);

    // TODO: design a better camera interface
    Vec3 lookFrom = Vec3(13.0f, 2.0f, 3.0f);
    Vec3 lookAt = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 cameraUp = Vec3(0.0f, 1.0f, 0.0f);
    float defocusAngle = 0.6f;
    float focalLength = 10.0f;
    float verticalFOV = 20.0f;      // vertical field of view in degrees

private:
    int imageWidth;
    int imageHeight;
    float aspectRatio;
    float viewportHeight;
    float viewportWidth;
    
    Vec3 cameraCenter;
    Vec3 viewportU;
    Vec3 viewportV;
    Vec3 pixelDeltaU;
    Vec3 pixelDeltaV;
    Vec3 viewportUpperLeft;
    Vec3 pixel00Loc;
    Vec3 u, v, w;           // camera frame basis vectors
    Vec3 defocusDiskU;
    Vec3 defocusDiskV;
};