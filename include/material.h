#pragma once

#include "utils.h"
#include "math/sampler.h"
#include "glm/glm.hpp"

enum class Flag : uint8_t {
    Diffuse,
    Specular,
    Transmission
};

struct BSDFSample {
    Vec3 BSDF; // future materials may require BSDF as a function of input and output rays
    Vec3 wi;
    float pdf = 0;
    float cosineFactor = 1; // required for Lambertian materials / materials with non Dirac delta-like reflections
    Flag flag;
};

// Rendering equation: Lo(p, wo) = Le(p, wo) + Int_Omega BSDF(p, wo, wi) * Li(p, wi) * |cos(theta_i)| dwi
// Monte Carlo estimator: 1/n Sum_i^n BSDF(p, wo, wi) * Li(p, wi) * |cos(theta_i)| / p(x_i)        

class Material {
public:
    virtual ~Material() = default;

    virtual BSDFSample sampleBSDF(const Vec3& wo, const Vec3& normal, Sampler* sampler, bool frontFace) const = 0;
    virtual Vec3 Le(const Vec3& wo, const Vec3& normal) const;
};

class Lambertian : public Material {
public:
    Lambertian(Vec3 color, Vec3 emissionColor = Vec3(0.0f));

    virtual BSDFSample sampleBSDF(const Vec3& wo, const Vec3& normal, Sampler* sampler, bool frontFace) const override;
    virtual Vec3 Le(const Vec3& wo, const Vec3& normal) const override;

private:
    Vec3 color;
    Vec3 emissionColor;
};

class Metal : public Material {
public:
    Metal(Vec3 color, float fuzz = 0.0f);

    virtual BSDFSample sampleBSDF(const Vec3& wo, const Vec3& normal, Sampler* sampler, bool frontFace) const override;
    static Vec3 reflect(const Vec3& in, const Vec3& normal);

private:
    Vec3 color;
    float fuzz;
};

class Dielectric : public Material {
public:
    Dielectric(float indexOfRefraction);

    virtual BSDFSample sampleBSDF(const Vec3& wo, const Vec3& normal, Sampler* sampler, bool frontFace) const override;

    // Returns the probability of reflection.
    static float fresnelReflectance(const Vec3& in, const Vec3& normal, float etaRatio, float* cosTheta_i, float* cosTheta_t);
    static Vec3 refract(const Vec3& in, const Vec3& normal, float etaRatio, float cosTheta_i, float cosTheta_t);

private:
    float indexOfRefraction;
};
