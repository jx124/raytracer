#include "renderer.h"

Renderer::Renderer(int width, int height) : width(width), height(height), image(width, height), camera(width, height) {
    image.init();
    
    scene.add(std::make_shared<Sphere>(Vec3(0.0f, 0.0f, -1.0f), 0.5f));
    scene.add(std::make_shared<Sphere>(Vec3(0.0f, -100.5f, -1.0f), 100.0f));
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
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            Ray cameraRay = camera.generateRay(i, j);
            image.writePixel(i, j, rayColor(cameraRay, scene));
        }
    }

    image.display();
}
