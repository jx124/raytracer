#include "renderer.h"

Renderer::Renderer(int width, int height) 
    : width(width), height(height), image(width, height), camera(width, height), sampler(32) {
    image.init();
    
    auto materialGround = std::make_shared<Lambertian>(Vec3(0.8f, 0.8f, 0.0f));
    auto materialCenter = std::make_shared<Lambertian>(Vec3(0.7f, 0.3f, 0.3f));
    auto materialLeft   = std::make_shared<Metal>(Vec3(0.8f, 0.8f, 0.8f), 0.1);
    auto materialRight  = std::make_shared<Metal>(Vec3(0.8f, 0.6f, 0.2f), 0.5);

    scene.add(std::make_shared<Sphere>(Vec3(0.0f, 0.0f, -1.0f), 0.5f, materialCenter));
    scene.add(std::make_shared<Sphere>(Vec3(-1.0f, 0.0f, -1.0f), 0.5f, materialLeft));
    scene.add(std::make_shared<Sphere>(Vec3(1.0f, 0.0f, -1.0f), 0.5f, materialRight));
    scene.add(std::make_shared<Sphere>(Vec3(0.0f, -100.5f, -1.0f), 100.0f, materialGround));
}

// Incident radiance
Vec3 Li(const Ray& ray, const Hittable& scene, Sampler* sampler, int depth) {
    if (depth == 0) {
        return Vec3(0.0f);
    }

    HitRecord rec;
    if (scene.hit(ray, Interval(0.0001f, infinity), rec)) {
        BSDFSample bs = rec.mat->sampleBSDF(ray.dir, rec.normal, sampler);
        Ray outRay(rec.point, bs.wi);

        return bs.BSDFCos * Li(outRay, scene, sampler, depth - 1) / bs.pdf;
    }

    Vec3 unitDirection = glm::normalize(ray.dir);
    float a = 0.5 * (unitDirection.y + 1.0f);
    return (1.0f - a) * Vec3(1.0f) + a * Vec3(0.5f, 0.7f, 1.0f);
}

void Renderer::onRender() {
    if (complete) {
        return;
    }

    Vec3 pixelRadiance(0.0f);
    for (int j = 0; j < height; j++) {
        if (j % 50 == 0) {
            std::printf("\rProgress: %d/%d (%.2f%%) complete", j, height, static_cast<float>(j * 100) / height);
            std::cout << std::flush;
        }

        for (int i = 0; i < width; i++) {
            pixelRadiance = {0.0f, 0.0f, 0.0f};
            for (int s = 0; s < sampler.getSamplesPerPixel(); s++) {
                Ray cameraRay = camera.generateRay(i, j, &sampler);
                pixelRadiance += Li(cameraRay, scene, &sampler, maxDepth);
            }
            image.writePixel(i, j, pixelRadiance / static_cast<float>(sampler.getSamplesPerPixel()));
        }
    }

    complete = true;
    std::cout << "\nRender complete" << std::endl;
    image.display();
}
