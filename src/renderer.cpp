#include "renderer.h"

Renderer::Renderer(int width, int height) 
        : width(width), height(height), image(width, height), camera(width, height) {

    image.init();
    sampler = std::make_unique<IndependentSampler>(256);
    scene = Scene::generateScene(sampler->clone().get(), camera, 3);

    scene.clear();
    scene.add(std::make_shared<BVHNode>(Scene::generateScene(sampler->clone().get(), camera, 3)));

    integrator = std::make_unique<SimplePathIntegrator>(scene);
}

void Renderer::onRender() {
    if (complete) {
        return;
    }

    auto startTime = std::chrono::high_resolution_clock::now();
    int linesComplete = 0;

    #pragma omp parallel default(none) firstprivate(camera) shared(sampler, linesComplete, std::cout, integrator)
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
