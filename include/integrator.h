#pragma once

#include "utils.h"
#include "hittables/hittable.h"
#include "hittables/scene.h"
#include "math/sampler.h"
#include "light.h"

constexpr int backgroundType = 1;
constexpr bool sampleLights = true;

class Integrator {
public:
    Integrator(Scene scene); // TODO: add lights
    virtual ~Integrator() = default;

    virtual Vec3 Li(const Ray& ray, Sampler* sampler, int depth) const = 0;
    static float powerHeuristic(int n, float nPDF, int m, float mPDF);

protected:
    Scene scene;
};

class RandomWalkIntegrator : public Integrator {
public:
    RandomWalkIntegrator(Scene scene);

    virtual Vec3 Li(const Ray& ray, Sampler* sampler, int depth) const override;
};

class SimplePathIntegrator : public Integrator {
public:
    SimplePathIntegrator(Scene scene);

    virtual Vec3 Li(const Ray& ray, Sampler* sampler, int depth) const override;

private:
    std::vector<std::shared_ptr<Light>> lights;
};

class PathIntegrator : public Integrator {
public:
    PathIntegrator(Scene scene);

    virtual Vec3 Li(const Ray& ray, Sampler* sampler, int depth) const override;

private:
    std::vector<std::shared_ptr<Light>> lights;
};