#include "integrator.h"

Integrator::Integrator(Scene scene) : scene(scene) {}

float Integrator::powerHeuristic(int n, float nPDF, int m, float mPDF) {
    float f = n * nPDF;
    float g = m * mPDF;
    return (f * f) / (f * f + g * g);
}

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

Vec3 SimplePathIntegrator::Li(const Ray &ray, Sampler *sampler, int depth) const {
    Vec3 L(0.0f);       // sampled radiance
    Vec3 beta(1.0f);    // path throughput weight: product of (BSDF * cos / pdf) terms
    bool specularBounce = true;
    HitRecord rec, lightRec;

    Ray newRay = ray;
    BSDFSample bs;
    LightSample ls;

    int bounces = 0;

    while (beta != Vec3(0.0f)) {
        if (!scene.hit(newRay, Interval(0.001f, infinity), rec)) {
            if (!sampleLights || specularBounce) {
                L += beta * scene.backgroundColor(newRay.dir, backgroundType);
            }
            break;
        }

        if (!sampleLights || specularBounce) {
            L += beta * rec.mat->Le(newRay.dir, rec.normal);
        }

        if (bounces++ == depth) {
            break;
        }

        bs = rec.mat->sampleBSDF(newRay.dir, rec.normal, sampler, rec.frontFace);

        if (sampleLights && lights.size() != 0 && bs.flag == Flag::Diffuse) {
            int sampledLightIndex = std::floor(sampler->get1D() * lights.size());
            ls = lights[sampledLightIndex]->sampleLight(rec.point, sampler);
            
            if (ls.pdf != 0) {
                Vec3 diff = ls.intersection - rec.point;
                scene.hit(Ray(rec.point, diff), Interval(0.001f, infinity), lightRec);

                bool unoccluded = glm::length(lightRec.point - ls.intersection) < 0.001f;
                if (unoccluded) {
                    L += beta * ls.L * bs.BSDF * std::abs(glm::dot(glm::normalize(diff), rec.normal)) 
                        * static_cast<float>(lights.size()) / (ls.pdf);
                }
            }
        }

        beta *= bs.BSDF * bs.cosineFactor / bs.pdf;
        newRay = Ray(rec.point, bs.wi);
        specularBounce = bs.flag != Flag::Diffuse;
    }

    return L;
}

PathIntegrator::PathIntegrator(Scene scene) : Integrator(scene), lights(scene.lights) {}

Vec3 PathIntegrator::Li(const Ray &ray, Sampler *sampler, int depth) const {
    Vec3 L(0.0f);       // sampled radiance
    Vec3 beta(1.0f);    // path throughput weight: product of (BSDF * cos / pdf) terms
    bool specularBounce = true;
    HitRecord rec, lightRec;

    Ray newRay = ray;
    BSDFSample bs;
    LightSample ls;

    int bounces = 0;

    while (true) {
        if (!scene.hit(newRay, Interval(0.001f, infinity), rec)) {
            if (!sampleLights || specularBounce) {
                L += beta * scene.backgroundColor(newRay.dir, backgroundType);
            }
            break;
        }

        if (!sampleLights || specularBounce) {
            L += beta * rec.mat->Le(newRay.dir, rec.normal);
        }

        if (bounces++ == depth) {
            break;
        }

        bs = rec.mat->sampleBSDF(newRay.dir, rec.normal, sampler, rec.frontFace);

        if (sampleLights && lights.size() != 0) {
            int sampledLightIndex = std::floor(sampler->get1D() * lights.size());
            ls = lights[sampledLightIndex]->sampleLight(rec.point, sampler);
            float lightPDF = ls.pdf / lights.size();
            
            // Light sampling
            if (ls.pdf != 0 && bs.flag == Flag::Diffuse) {
                Vec3 diff = ls.intersection - rec.point;
                scene.hit(Ray(rec.point, diff), Interval(0.001f, infinity), lightRec);

                bool unoccluded = glm::length(lightRec.point - ls.intersection) < 0.001f;
                if (unoccluded) {
                    Vec3 f = bs.BSDF * std::abs(glm::dot(glm::normalize(diff), rec.normal));
                    float weight = powerHeuristic(1, lightPDF, 1, bs.pdf);

                    L += beta * ls.L * f * weight / lightPDF;
                }
            }

            // BRDF sampling
            if (bs.pdf != 0) {
                float weight = powerHeuristic(1, bs.pdf, 1, lightPDF);
                L += beta * bs.BSDF * bs.cosineFactor * weight / bs.pdf;
            }
        }

        beta *= bs.BSDF * bs.cosineFactor / bs.pdf;
        newRay = Ray(rec.point, bs.wi);
        specularBounce = bs.flag != Flag::Diffuse;

        // Russian roulette path termination
        float RRProb = std::max(beta.x, std::max(beta.y, beta.z));
        if (bounces > 3) {
            if (sampler->get1D() > RRProb) {
                break;
            }
            beta *= 1 / RRProb;
        }
    }

    return L;
}
