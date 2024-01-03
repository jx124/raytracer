#include "math/sampler.h"

Sampler::Sampler(int samplesPerPixel, int seed) : samplesPerPixel(samplesPerPixel), seed(seed), rng(seed) {}

int Sampler::getSamplesPerPixel() const { 
    return samplesPerPixel;
}

void Sampler::setSeed(int newSeed) { 
    rng = RNG(newSeed);
}

Vec2 sampleUniformDiskConcentric(Vec2 u) {
    Vec2 uOffset = 2.0f * u - Vec2(1.0f);
    if (uOffset.x == 0.0f && uOffset.y == 0.0f) {
        return Vec2(0.0f);
    }

    float theta, r;
    if (std::abs(uOffset.x) > std::abs(uOffset.y)) {
        r = uOffset.x;
        theta = (pi / 4.0f) * (uOffset.y / uOffset.x);
    } else {
        r = uOffset.y;
        theta = (pi / 2.0f) - (pi / 4.0f) * (uOffset.x / uOffset.y);
    }
    return r * Vec2(std::cos(theta), std::sin(theta));
}

Vec3 sampleUniformSphere(Vec2 u) {
    float z = 1 - 2 * u.x;
    float r = std::sqrt(1 - z * z);
    float phi = 2 * pi * u.y;
    return {r * std::cos(phi), r * std::sin(phi), z};
}

Vec3 sampleUniformHemisphere(Vec2 u) {
    float z = u.x;
    float r = std::sqrt(1 - z * z);
    float phi = 2 * pi * u.y;
    return {r * std::cos(phi), r * std::sin(phi), z};
}

Vec3 sampleCosineHemisphere(Vec2 u) {
    Vec2 d = sampleUniformDiskConcentric(u);
    float z = std::sqrt(1 - d.x * d.x - d.y * d.y);
    return {d.x, d.y, z};
}

IndependentSampler::IndependentSampler(int samplesPerPixel, int seed) : Sampler(samplesPerPixel, seed) {}

float IndependentSampler::get1D() { 
    return rng.uniform<float>();
}

Vec2 IndependentSampler::get2D() {
    return {rng.uniform<float>(), rng.uniform<float>()};
}

Vec3 IndependentSampler::get3D() {
    return {rng.uniform<float>(), rng.uniform<float>(), rng.uniform<float>()};
}

Vec2 IndependentSampler::get2DPixel() {
    return get2D();
}

std::unique_ptr<Sampler> IndependentSampler::clone() const {
    return std::make_unique<IndependentSampler>(samplesPerPixel, seed);
};
