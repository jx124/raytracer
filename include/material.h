#pragma once

#include "utils.h"
#include "sampler.h"
#include "glm/glm.hpp"

struct BSDFSample {
    Vec3 BSDFCos;
    Vec3 wi;
    float pdf = 0;
};

// Rendering equation: Lo(p, wo) = Le(p, wo) + Int_Omega BSDF(p, wo, wi) * Li(p, wi) * |cos(theta_i)| dwi
// Monte Carlo estimator: 1/n Sum_i^n BSDF(p, wo, wi) * Li(p, wi) * |cos(theta_i)| / p(x_i)        

class Material {
public:
    virtual ~Material() = default;
    virtual BSDFSample sampleBSDF(const Vec3&, const Vec3& normal, Sampler* sampler) const = 0;
};

class Lambertian : public Material {
public:
    Lambertian(Vec3 color) : color(color) {};

    virtual BSDFSample sampleBSDF(const Vec3& wo, const Vec3& normal, Sampler* sampler) const override {
        Vec3 wi = sampleUniformSphere(sampler->get2DPixel()) + normal;

        if (glm::dot(wi, wi) < 1e-8f) {
            wi = normal;
        }

        float cosTheta = glm::dot(wi, normal);
        float pdf = cosineHemispherePDF(cosTheta);        

        Vec3 BSDFCos = color * cosTheta / pi;

        return {BSDFCos, wi, pdf};
    }

private:
    Vec3 color;
};

class Metal : public Material {
public:
    Metal(Vec3 color, float fuzz = 0) : color(color), fuzz(fuzz) {};

    virtual BSDFSample sampleBSDF(const Vec3& wo, const Vec3& normal, Sampler* sampler) const override {
        Vec3 wi = reflect(wo, normal) + fuzz * sampleUniformSphere(sampler->get2DPixel());
        float pdf = 1.0f;        

        Vec3 BSDFCos = color;

        return {BSDFCos, wi, pdf};
    }

private:
    static Vec3 reflect(const Vec3& in, const Vec3& normal) {
        return in - 2 * glm::dot(in, normal) * normal;
    }

    Vec3 color;
    float fuzz;
};