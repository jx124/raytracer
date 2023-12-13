#include "renderer.h"

Renderer::Renderer(int width, int height) : width(width), height(height), image(width, height) {
    image.init();
}

float hitSphere(const Vec3& center, float radius, const Ray& ray) {
    Vec3 oc = ray.orig - center;
    float a = glm::dot(ray.dir, ray.dir);
    float halfB = glm::dot(oc, ray.dir);
    float c = glm::dot(oc, oc) - radius * radius;
    float discriminant = halfB * halfB - a * c;
    if (discriminant < 0) {
        return -1.0f;
    } else {
        return (-halfB - std::sqrt(discriminant)) / a;
    }
}

Vec3 rayColor(const Ray& ray) {
    float t = hitSphere(Vec3(0.0f, 0.0f, -1.0f), 0.5f, ray);
    if (t > 0.0f) {
        Vec3 normal = glm::normalize(ray.at(t) - Vec3(0.0f, 0.0f, -1.0f));

        return 0.5f * Vec3(normal.x + 1, normal.y + 1, normal.z + 1);
    }
    Vec3 unitDirection = glm::normalize(ray.dir);
    float a = 0.5 * (unitDirection.y + 1.0f);
    return (1.0f - a) * Vec3(1.0f) + a * Vec3(0.5f, 0.7f, 1.0f);
}

void Renderer::onRender() {
    // Camera
    float aspectRatio = static_cast<float>(width) / height;
    float focalLength = 1.0f;
    float viewportHeight = 2.0f;
    float viewportWidth = viewportHeight * aspectRatio;
    Vec3 cameraCenter = Vec3(0.0f);

    Vec3 viewportU = Vec3(viewportWidth, 0.0f, 0.0f);
    Vec3 viewportV = Vec3(0.0f, -viewportHeight, 0.0f);

    Vec3 pixelDeltaU = viewportU / static_cast<float>(width);
    Vec3 pixelDeltaV = viewportV / static_cast<float>(height);
    
    Vec3 viewportUpperLeft = cameraCenter - Vec3(0.0f, 0.0f, focalLength) - viewportU / 2.0f - viewportV / 2.0f;
    Vec3 pixel00Loc = viewportUpperLeft + 0.5f * (pixelDeltaU + pixelDeltaV);

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            Vec3 pixelCenter = pixel00Loc + static_cast<float>(i) * pixelDeltaU + static_cast<float>(j) * pixelDeltaV;
            Vec3 rayDirection = pixelCenter - cameraCenter;
            Ray ray(cameraCenter, rayDirection);

            image.writePixel(i, j, rayColor(ray));
        }
    }

    image.display();
}
