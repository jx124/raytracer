#include "integrator.h"

#include <iostream>

Integrator::Integrator(Scene scene) : scene(scene) {}

RandomWalkIntegrator::RandomWalkIntegrator(Scene scene) : Integrator(scene) {}

Vec3 RandomWalkIntegrator::Li(const Ray &ray, Sampler *sampler, int depth) const {
    if (depth == 0) {
        return Vec3(0.0f);
    }

    HitRecord rec;
    if (scene.hit(ray, Interval(0.001f, infinity), rec)) {
        BSDFSample bs = rec.mat->sampleBSDF(ray.dir, rec.normal, sampler, rec.frontFace);
        Ray outRay(rec.point, bs.wi);

        return bs.BSDF * Li(outRay, sampler, depth - 1) * bs.cosineFactor / bs.pdf + rec.mat->Le(ray.dir, rec.normal);
    }

    return scene.backgroundColor(ray.dir, backgroundType);
}

SimplePathIntegrator::SimplePathIntegrator(Scene scene) : Integrator(scene), lights(scene.lights) {}

constexpr bool sampleLights = true;

Vec3 SimplePathIntegrator::Li(const Ray &ray, Sampler *sampler, int depth) const {
    Vec3 L(0.0f);       // sampled radiance
    Vec3 beta(1.0f);    // path throughput weight: product of (BSDF * cos / pdf) terms
    bool specularBounce = true;
    HitRecord rec;
    int lightSamples = 0;

    Ray rayCopy = ray;

    while (beta != Vec3(0.0f)) {
        bool hit = scene.hit(rayCopy, Interval(0.001f, infinity), rec);

        if (!hit) {
            if (!sampleLights || specularBounce) {
                L += beta * scene.backgroundColor(rayCopy.dir, backgroundType);
            }
            break;
        }

        if (!sampleLights || specularBounce) {
            L += beta * rec.mat->Le(rayCopy.dir, rec.normal);
        }

        if (depth-- == 0) {
            break;
        }

        BSDFSample bs = rec.mat->sampleBSDF(rayCopy.dir, rec.normal, sampler, rec.frontFace);

        if (sampleLights && lights.size() != 0) {
            int sampledLightIndex = std::floor(sampler->get1D() * lights.size());
            LightSample ls = lights[sampledLightIndex]->sampleLight(rec.point, sampler);
            
            if (ls.pdf != 0) {
                HitRecord lightRec;
                scene.hit(Ray(rec.point, ls.intersection - rec.point), Interval(0.001f, infinity), lightRec);

                bool unoccluded = glm::length(lightRec.point - ls.intersection) < 0.001f;
                if (unoccluded) {
                    L += beta * ls.L * bs.BSDF * std::abs(glm::dot(glm::normalize(ls.intersection - rec.point), rec.normal)) / (ls.pdf);
                    lightSamples++;
                }
            }
        }

        beta *= bs.BSDF * bs.cosineFactor / bs.pdf;
        rayCopy = Ray(rec.point, bs.wi);
        specularBounce = bs.flag == Flag::Specular;
    }

    return L * (static_cast<float>(sampler->getSamplesPerPixel()) / (sampler->getSamplesPerPixel() + lightSamples));
}
