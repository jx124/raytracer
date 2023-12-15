#pragma once

#include "utils.h"
#include "sampler.h"

class Camera {
public:
    Camera(int width, int height) : width(width), height(height) {
        aspectRatio = static_cast<float>(width) / height;
        focalLength = 1.0f;
        viewportHeight = 2.0f;
        viewportWidth = viewportHeight * aspectRatio;
        cameraCenter = Vec3(0.0f);

        viewportU = Vec3(viewportWidth, 0.0f, 0.0f);
        viewportV = Vec3(0.0f, -viewportHeight, 0.0f);

        pixelDeltaU = viewportU / static_cast<float>(width);
        pixelDeltaV = viewportV / static_cast<float>(height);
        
        viewportUpperLeft = cameraCenter - Vec3(0.0f, 0.0f, focalLength) - viewportU / 2.0f - viewportV / 2.0f;
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

private:
    int width;
    int height;

    float aspectRatio;
    float focalLength;
    float viewportHeight;
    float viewportWidth;
    
    Vec3 cameraCenter;
    Vec3 viewportU;
    Vec3 viewportV;
    Vec3 pixelDeltaU;
    Vec3 pixelDeltaV;
    Vec3 viewportUpperLeft;
    Vec3 pixel00Loc;
};