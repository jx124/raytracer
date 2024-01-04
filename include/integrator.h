#pragma once

#include "utils.h"
#include "hittables/hittable.h"
#include "hittables/scene.h"
#include "math/sampler.h"
#include "light.h"

constexpr int backgroundType = 1;

class Integrator {
public:
    Integrator(Scene scene); // TODO: add lights
    virtual ~Integrator() = default;

    virtual Vec3 Li(const Ray& ray, Sampler* sampler, int depth) const = 0;

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