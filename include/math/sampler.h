#pragma once

#include "utils.h"
#include "rng.h"
#include "glm/vec2.hpp"

using Vec2 = glm::vec2;

class Sampler {
public:
    Sampler(int samplesPerPixel, int seed = 0);
    virtual ~Sampler() = default;

    int getSamplesPerPixel() const;
    void setSeed(int newSeed);

    virtual float get1D() = 0;
    virtual Vec2 get2D() = 0;
    virtual Vec3 get3D() = 0;
    virtual Vec2 get2DPixel() = 0;
    virtual std::unique_ptr<Sampler> clone() const = 0;

protected:
    int samplesPerPixel;
    int seed;
    RNG rng;
};

class IndependentSampler : public Sampler {
public:
    IndependentSampler(int samplesPerPixel, int seed = 0);

    virtual float get1D() override;
    virtual Vec2 get2D() override;
    virtual Vec3 get3D() override;
    virtual Vec2 get2DPixel() override;
    virtual std::unique_ptr<Sampler> clone() const override;
};

Vec2 sampleUniformDiskConcentric(Vec2 u);
Vec3 sampleUniformSphere(Vec2 u);
Vec3 sampleUniformHemisphere(Vec2 u);
Vec3 sampleCosineHemisphere(Vec2 u);

constexpr float uniformSpherePDF() {
    return 1.0f / (4 * pi);
}

constexpr float uniformHemipherePDF() {
    return 1.0f / (2 * pi);
}

constexpr float cosineHemispherePDF(float cosTheta) {
    return cosTheta / pi;
}