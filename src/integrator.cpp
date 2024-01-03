#include "integrator.h"

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

        return bs.BSDFCos * Li(outRay, sampler, depth - 1) / bs.pdf + rec.mat->Le(ray.dir, rec.normal);
    }

    return scene.backgroundColor(ray.dir, 1);
}