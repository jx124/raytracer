#include "renderer.h"

Renderer::Renderer(int width, int height) : width(width), height(height), image(width, height) {
    image.init();
}

Vec3 rayColor(const Ray& ray, const Hittable& scene) {
    HitRecord rec;
    if (scene.hit(ray, Interval(0, infinity), rec)) {
        return 0.5f * (rec.normal + 1.0f);
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

    Scene scene;
    scene.add(std::make_shared<Sphere>(Vec3(0.0f, 0.0f, -1.0f), 0.5f));
    scene.add(std::make_shared<Sphere>(Vec3(0.0f, -100.5f, -1.0f), 100.0f));

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            Vec3 pixelCenter = pixel00Loc + static_cast<float>(i) * pixelDeltaU + static_cast<float>(j) * pixelDeltaV;
            Vec3 rayDirection = pixelCenter - cameraCenter;
            Ray ray(cameraCenter, rayDirection);

            image.writePixel(i, j, rayColor(ray, scene));
        }
    }

    image.display();
}
