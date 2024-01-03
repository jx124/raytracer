#include "renderer.h"

Renderer::Renderer(int width, int height) 
        : width(width), height(height), image(width, height), camera(width, height) {

    image.init();
    scene = Scene(std::make_shared<BVHNode>(generateScene(3)));
    sampler = std::make_unique<IndependentSampler>(32);
    integrator = std::make_unique<RandomWalkIntegrator>(scene);
}

void Renderer::onRender() {
    if (complete) {
        return;
    }

    auto startTime = std::chrono::high_resolution_clock::now();
    int linesComplete = 0;

    #pragma omp parallel default(none) firstprivate(camera) shared(sampler, linesComplete, std::cout)
    {
        std::unique_ptr<Sampler> samplerCloneUnique;

        #pragma omp critical
        samplerCloneUnique = sampler->clone();

        Sampler* samplerClone = samplerCloneUnique.get();
        samplerClone->setSeed(omp_get_thread_num());

        Vec3 pixelRadiance(0.0f);

        #pragma omp for schedule(dynamic) 
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                pixelRadiance = {0.0f, 0.0f, 0.0f};
                for (int s = 0; s < samplerClone->getSamplesPerPixel(); s++) {
                    Ray cameraRay = camera.generateRay(i, j, samplerClone);
                    pixelRadiance += integrator->Li(cameraRay, samplerClone, maxDepth);
                }
                image.writePixel(i, j, pixelRadiance / static_cast<float>(sampler->getSamplesPerPixel()));
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
    }

    auto endTime = std::chrono::high_resolution_clock::now();

    complete = true;
    std::cout << "\nRender complete, time taken: " 
        << static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()) / 1000
        << " s" << std::endl;
}

void Renderer::displayImage() {
    image.display();
};

Scene randomSpheres(Sampler* sampler, Camera& camera) {
    Scene scene;
    auto groundMaterial = std::make_shared<Lambertian>(Vec3(0.5f, 0.5f, 0.5f));
    scene.add(std::make_shared<Sphere>(Vec3(0.0f, -1000.0f, 0.0f), 1000.0f, groundMaterial));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float chooseMaterial = sampler->get1D();
            Vec3 center(a + 0.9f * sampler->get1D(), 0.2f, b + 0.9f * sampler->get1D());

            if (glm::length(center - Vec3(4.0f, 0.2f, 0.0f)) > 0.9f) {
                std::shared_ptr<Material> sphereMaterial;

                if (chooseMaterial < 0.8) {
                    // diffuse
                    sphereMaterial = std::make_shared<Lambertian>(sampler->get3D() * sampler->get3D());
                } else if (chooseMaterial < 0.95) {
                    // metal
                    sphereMaterial = std::make_shared<Metal>(sampler->get3D() / 2.0f + Vec3(0.5f),
                                                            sampler->get1D() / 2.0f);
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

    camera.lookFrom = Vec3(13.0f, 2.0f, 3.0f);
    camera.lookAt = Vec3(0.0f, 0.0f, 0.0f);
    camera.cameraUp = Vec3(0.0f, 1.0f, 0.0f);
    camera.defocusAngle = 0.6f;
    camera.focalLength = 10.0f;
    camera.verticalFOV = 20.0f;

    camera.init();

    return scene;
}

Scene quads(Camera& camera) {
    Scene scene;
    // Materials
    auto left_red     = std::make_shared<Lambertian>(Vec3(1.0, 0.2, 0.2));
    auto back_green   = std::make_shared<Lambertian>(Vec3(0.2, 1.0, 0.2));
    auto right_blue   = std::make_shared<Lambertian>(Vec3(0.2, 0.2, 1.0));
    auto upper_orange = std::make_shared<Lambertian>(Vec3(1.0, 0.5, 0.0));
    auto lower_teal   = std::make_shared<Lambertian>(Vec3(0.2, 0.8, 0.8));

    // Quads
    scene.add(std::make_shared<Quad>(Vec3(-3,-2, 5), Vec3(0, 0,-4), Vec3(0, 4, 0), left_red));
    scene.add(std::make_shared<Quad>(Vec3(-2,-2, 0), Vec3(4, 0, 0), Vec3(0, 4, 0), back_green));
    scene.add(std::make_shared<Quad>(Vec3( 3,-2, 1), Vec3(0, 0, 4), Vec3(0, 4, 0), right_blue));
    scene.add(std::make_shared<Quad>(Vec3(-2, 3, 1), Vec3(4, 0, 0), Vec3(0, 0, 4), upper_orange));
    scene.add(std::make_shared<Quad>(Vec3(-2,-3, 5), Vec3(4, 0, 0), Vec3(0, 0,-4), lower_teal));

    camera.lookFrom = Vec3(0.0f, 0.0f, 9.0f);
    camera.lookAt = Vec3(0.0f, 0.0f, 0.0f);
    camera.cameraUp = Vec3(0.0f, 1.0f, 0.0f);
    camera.defocusAngle = 0.01f;
    camera.focalLength = 10.0f;
    camera.verticalFOV = 80.0f;

    camera.init();

    return scene;
}

Scene cornellBox(Camera& camera) {
    Scene scene;

    auto red   = std::make_shared<Lambertian>(Vec3(0.65f, 0.05f, 0.05f));
    auto white = std::make_shared<Lambertian>(Vec3(0.73f, 0.73f, 0.73f));
    auto green = std::make_shared<Lambertian>(Vec3(0.12f, 0.45f, 0.15f));
    auto light = std::make_shared<Lambertian>(Vec3(1.0f), Vec3(15.0f));

    scene.add(std::make_shared<Quad>(Vec3(555, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), green));
    scene.add(std::make_shared<Quad>(Vec3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), red));
    scene.add(std::make_shared<Quad>(Vec3(343, 554, 332), Vec3(-130, 0, 0), Vec3(0, 0, -105), light));
    scene.add(std::make_shared<Quad>(Vec3(0, 0, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
    scene.add(std::make_shared<Quad>(Vec3(555, 555, 555), Vec3(-555, 0, 0), Vec3(0, 0, -555), white));
    scene.add(std::make_shared<Quad>(Vec3(0, 555, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
    scene.add(std::make_shared<Quad>(Vec3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 555, 0), white));

    camera.verticalFOV = 40.0f;
    camera.lookFrom = Vec3(278, 278, -800);
    camera.lookAt = Vec3(278, 278, 0);
    camera.cameraUp = Vec3(0,1,0);
    camera.defocusAngle = 0.0f;

    camera.init();

    return scene;
}

Scene Renderer::generateScene(int index) {
    switch (index) {
    case 1:
        return randomSpheres(sampler.get(), camera);
    case 2:
        return quads(camera);
    case 3: default:
        return cornellBox(camera);
    }
}
