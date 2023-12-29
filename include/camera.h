#pragma once

#include "utils.h"
#include "sampler.h"

class Camera {
public:
    Camera(int imageWidth, int imageHeight, float verticalFOV = 20.0f)
            : imageWidth(imageWidth), imageHeight(imageHeight), verticalFOV(verticalFOV) {

        aspectRatio = static_cast<float>(imageWidth) / imageHeight;
        cameraCenter = lookFrom;

        float theta = verticalFOV * pi / 180.0f; // convert to radians
        float h = std::tan(theta / 2.0f);

        // focalLength = glm::length(lookFrom - lookAt);
        viewportHeight = 2.0f * h * focalLength;
        viewportWidth = viewportHeight * aspectRatio;

        w = glm::normalize(lookFrom - lookAt);
        u = glm::normalize(glm::cross(cameraUp, w));
        v = glm::cross(w, u);

        viewportU = viewportWidth * u;
        viewportV = -viewportHeight * v;

        pixelDeltaU = viewportU / static_cast<float>(imageWidth);
        pixelDeltaV = viewportV / static_cast<float>(imageHeight);
        
        viewportUpperLeft = cameraCenter - (focalLength * w) - viewportU / 2.0f - viewportV / 2.0f;
        pixel00Loc = viewportUpperLeft + 0.5f * (pixelDeltaU + pixelDeltaV);

        theta = defocusAngle * pi / 180.0f;
        float defocusRadius = focalLength * std::tan(theta / 2.0f);
        defocusDiskU = u * defocusRadius;
        defocusDiskV = v * defocusRadius;
    }

    Ray generateRay(int i, int j, Sampler* sampler) {
        // TODO: implement better filters to replace current box filter
        Vec2 filterSample = sampler->get2DPixel() - Vec2(0.5f, 0.5f);
        Vec3 pixelCenter = pixel00Loc + static_cast<float>(i) * pixelDeltaU + static_cast<float>(j) * pixelDeltaV;
        Vec3 pixelSample = pixelCenter + filterSample.x * pixelDeltaU + filterSample.y * pixelDeltaV;

        Vec2 defocus = sampleUniformDiskConcentric(sampler->get2DPixel());
        Vec3 rayOrigin = cameraCenter + defocus.x * defocusDiskU + defocus.y * defocusDiskV;
        Vec3 rayDirection = pixelSample - rayOrigin;

        return Ray(rayOrigin, rayDirection);
    }

    // TODO: design a better camera interface
    Vec3 lookFrom = Vec3(13.0f, 2.0f, 3.0f);
    Vec3 lookAt = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 cameraUp = Vec3(0.0f, 1.0f, 0.0f);
    float defocusAngle = 0.6f;
    float focalLength = 10.0f;

private:
    int imageWidth;
    int imageHeight;

    float aspectRatio;
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
    Vec3 defocusDiskU;
    Vec3 defocusDiskV;
};