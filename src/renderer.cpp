#include "renderer.h"

Renderer::Renderer(int width, int height) 
: width(width), height(height), image(width, height), camera(width, height), sampler(32) {
    image.init();
    
    scene.add(std::make_shared<Sphere>(Vec3(0.0f, 0.0f, -1.0f), 0.5f));
    scene.add(std::make_shared<Sphere>(Vec3(0.0f, -100.5f, -1.0f), 100.0f));
}

// Incident radiance
Vec3 Li(const Ray& ray, const Hittable& scene, Sampler* sampler, int depth) {
    if (depth == 0) {
        return Vec3(0.0f);
    }

    HitRecord rec;
    if (scene.hit(ray, Interval(0.0001f, infinity), rec)) {
        // Rendering equation: Lo(p, wo) = Le(p, wo) + Int_Omega BSDF(p, wo, wi) * Li(p, wi) * |cos(theta_i)| dwi
        // Monte Carlo estimator: 1/n Sum_i^n BSDF(p, wo, wi) * Li(p, wi) * |cos(theta_i)| / p(x_i)
        
        // Vec3 wo = -ray.dir;
        Vec3 wp = sampleCosineHemisphere(sampler->get2DPixel());
        float cosTheta = glm::dot(wp, rec.normal);

        Vec3 BSDFCos = Vec3(0.5f / pi) * cosTheta;
        
        // if BSDFCos == 0 return Le
        Ray outRay(rec.point, wp);

        return BSDFCos * Li(outRay, scene, sampler, depth - 1) / cosineHemispherePDF(cosTheta);
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
