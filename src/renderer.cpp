#include "renderer.h"

Renderer::Renderer(int width, int height) 
        : width(width), height(height), image(width, height), camera(width, height), sampler(16, 1234) {
    image.init();

    auto groundMaterial = std::make_shared<Lambertian>(Vec3(0.5f, 0.5f, 0.5f));
    scene.add(std::make_shared<Sphere>(Vec3(0.0f, -1000.0f, 0.0f), 1000.0f, groundMaterial));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float chooseMaterial = sampler.get1D();
            Vec3 center(a + 0.9f * sampler.get1D(), 0.2f, b + 0.9f * sampler.get1D());

            if (glm::length(center - Vec3(4.0f, 0.2f, 0.0f)) > 0.9f) {
                std::shared_ptr<Material> sphereMaterial;

                if (chooseMaterial < 0.8) {
                    // diffuse
                    sphereMaterial = std::make_shared<Lambertian>(sampler.get3D() * sampler.get3D());
                } else if (chooseMaterial < 0.95) {
                    // metal
                    sphereMaterial = std::make_shared<Metal>(sampler.get3D() / 2.0f + Vec3(0.5f),
                                                             sampler.get1D() / 2.0f);
                } else {
                    // glass
                    sphereMaterial = std::make_shared<Dielectric>(1.5f);
                }
                scene.add(std::make_shared<Sphere>(center, 0.2f, sphereMaterial));
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5f);
    scene.add(std::make_shared<Sphere>(Vec3(0.0f, 1.0f, 0.0f), 1.0f, material1));

    auto material2 = std::make_shared<Lambertian>(Vec3(0.4f, 0.2f, 0.1f));
    scene.add(std::make_shared<Sphere>(Vec3(-4.0f, 1.0f, 0.0f), 1.0f, material2));

    auto material3 = std::make_shared<Metal>(Vec3(0.7f, 0.6f, 0.5f));
    scene.add(std::make_shared<Sphere>(Vec3(4.0f, 1.0f, 0.0f), 1.0f, material3));

    scene = Scene(std::make_shared<BVHNode>(scene));
}

// Incident radiance
Vec3 Li(const Ray& ray, const Hittable& scene, Sampler* sampler, int depth) {
    if (depth == 0) {
        return Vec3(0.0f);
    }

    HitRecord rec;
    if (scene.hit(ray, Interval(0.001f, infinity), rec)) {
        BSDFSample bs = rec.mat->sampleBSDF(ray.dir, rec.normal, sampler, rec.frontFace);
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

    auto startTime = std::chrono::high_resolution_clock::now();

    Vec3 pixelRadiance(0.0f);
    int linesComplete = 0;

    #pragma omp parallel for schedule(dynamic) default(shared) firstprivate(sampler, camera, pixelRadiance) 
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            pixelRadiance = {0.0f, 0.0f, 0.0f};
            for (int s = 0; s < sampler.getSamplesPerPixel(); s++) {
                Ray cameraRay = camera.generateRay(i, j, &sampler);
                pixelRadiance += Li(cameraRay, scene, &sampler, maxDepth);
            }
            image.writePixel(i, j, pixelRadiance / static_cast<float>(sampler.getSamplesPerPixel()));
        }

        #pragma omp atomic
        linesComplete++;
    
        #pragma omp critical
        if (linesComplete % 50 == 0) {
            std::printf("\rProgress: %d/%d (%.2f%%) complete", linesComplete, height, 
                static_cast<float>(linesComplete * 100) / height);
            std::cout << std::flush;
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();

    complete = true;
    std::cout << "\nRender complete, time taken: " 
        << static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()) / 1000
        << " s" << std::endl;
}
