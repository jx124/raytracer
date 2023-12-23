#pragma once

#include "utils.h"
#include "sampler.h"

class Camera {
public:
    Camera(int width, int height, float verticalFOV = 25.0f)
            : width(width), height(height), verticalFOV(verticalFOV) {
        aspectRatio = static_cast<float>(width) / height;

        cameraCenter = lookFrom;
        float theta = verticalFOV * pi / 180.0f; // convert to radians
        float h = std::tan(theta / 2.0f);

        focalLength = glm::length(lookFrom - lookAt);
        viewportHeight = 2.0f * h * focalLength;
        viewportWidth = viewportHeight * aspectRatio;

        w = glm::normalize(lookFrom - lookAt);
        u = glm::normalize(glm::cross(cameraUp, w));
        v = glm::cross(w, u);

        viewportU = viewportWidth * u;
        viewportV = -viewportHeight * v;

        pixelDeltaU = viewportU / static_cast<float>(width);
        pixelDeltaV = viewportV / static_cast<float>(height);
        
        viewportUpperLeft = cameraCenter - (focalLength * w) - viewportU / 2.0f - viewportV / 2.0f;
        pixel00Loc = viewportUpperLeft + 0.5f * (pixelDeltaU + pixelDeltaV);
    }

    Ray generateRay(int i, int j, Sampler* sampler) {
        // TODO: implement better filters to replace current box filter
        Vec2 filter = sampler->get2D() - Vec2(0.5f, 0.5f);
        Vec3 pixelCenter = pixel00Loc + static_cast<float>(i) * pixelDeltaU + static_cast<float>(j) * pixelDeltaV;

        pixelCenter += Vec3(filter.x * pixelDeltaU.x, filter.y * pixelDeltaV.y, 0.0f);
        
        Vec3 rayDirection = pixelCenter - cameraCenter;
        return Ray(cameraCenter, rayDirection);
    }

    // TODO: design a better camera interface
    Vec3 lookFrom = Vec3(-2.0f, 2.0f, 1.0f);
    Vec3 lookAt = Vec3(0.0f, 0.0f, -1.0f);
    Vec3 cameraUp = Vec3(0.0f, 1.0f, 0.0f);

private:
    int width;
    int height;

    float aspectRatio;
    float focalLength;
    float viewportHeight;
    float viewportWidth;
    float verticalFOV;      // vertical field of view in degrees
    
    Vec3 cameraCenter;
    Vec3 viewportU;
    Vec3 viewportV;
    Vec3 pixelDeltaU;
    Vec3 pixelDeltaV;
    Vec3 viewportUpperLeft;
    Vec3 pixel00Loc;
    Vec3 u, v, w;           // camera frame basis vectors
};