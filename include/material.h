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
    virtual BSDFSample sampleBSDF(const Vec3& wo, const Vec3& normal, Sampler* sampler, bool frontFace) const = 0;
    virtual Vec3 Le(const Vec3& wo, const Vec3& normal) const {
        (void) wo;
        (void) normal;
        return Vec3(0.0f);
    }
};

class Lambertian : public Material {
public:
    Lambertian(Vec3 color, Vec3 emissionColor = Vec3(0.0f)) : color(color), emissionColor(emissionColor) {};

    virtual BSDFSample sampleBSDF(const Vec3& wo, const Vec3& normal, Sampler* sampler, bool frontFace) const override {
        (void) wo;
        (void) frontFace;
        Vec3 wi = sampleUniformSphere(sampler->get2DPixel()) + normal;

        if (glm::dot(wi, wi) < 1e-8f) {
            wi = normal;
        }

        float cosTheta = glm::dot(wi, normal);
        float pdf = cosineHemispherePDF(cosTheta);        

        Vec3 BSDFCos = color * cosTheta / pi;

        return {BSDFCos, wi, pdf};
    }

    virtual Vec3 Le(const Vec3& wo, const Vec3& normal) const override {
        (void) wo;
        (void) normal; // For future use in directional lighting?
        return emissionColor;
    }

private:
    Vec3 color;
    Vec3 emissionColor;
};

class Metal : public Material {
public:
    Metal(Vec3 color, float fuzz = 0) : color(color), fuzz(fuzz) {};

    virtual BSDFSample sampleBSDF(const Vec3& wo, const Vec3& normal, Sampler* sampler, bool frontFace) const override {
        (void) frontFace;
        Vec3 BSDFCos = color;
        Vec3 wi = reflect(wo, normal) + fuzz * sampleUniformSphere(sampler->get2DPixel());
        float pdf = 1.0f;

        return {BSDFCos, wi, pdf};
    }

    static Vec3 reflect(const Vec3& in, const Vec3& normal) {
        return in - 2 * glm::dot(in, normal) * normal;
    }

private:
    Vec3 color;
    float fuzz;
};

class Dielectric : public Material {
public:
    Dielectric(float indexOfRefraction) : indexOfRefraction(indexOfRefraction) {}

    virtual BSDFSample sampleBSDF(const Vec3& wo, const Vec3& normal, Sampler* sampler, bool frontFace) const override {
        Vec3 in = -glm::normalize(wo);
        float etaRatio = indexOfRefraction;

        if (!frontFace) {
            etaRatio = 1.0f / etaRatio;
        }

        float cosTheta_i, cosTheta_t;
        float R = fresnelReflectance(in, normal, etaRatio, &cosTheta_i, &cosTheta_t);
        float T = 1.0f - R;

        Vec3 BSDFCos, wi;
        float pdf;

        if (sampler->get1D() < R) {
            // reflect
            BSDFCos = Vec3(R);
            wi = Metal::reflect(wo, normal);
            pdf = R;
        } else {
            // transmit
            BSDFCos = Vec3(T / (etaRatio * etaRatio));
            wi = refract(in, normal, etaRatio, cosTheta_i, cosTheta_t);
            pdf = T;
        }

        return {BSDFCos, wi, pdf};
    }

    // Returns the probability of reflection.
    static float fresnelReflectance(const Vec3& in, const Vec3& normal, 
                                    float etaRatio, float* cosTheta_i, float* cosTheta_t) {
        float tempCosTheta_i = std::abs(glm::dot(in, normal));

        float sin2Theta_i = 1.0f - (tempCosTheta_i * tempCosTheta_i);
        float sin2Theta_t = sin2Theta_i / (etaRatio * etaRatio);
        if (sin2Theta_t >= 1.0f) {
            *cosTheta_i = tempCosTheta_i;
            *cosTheta_t = 0.0f;
            return 1.0f;
        }
        float tempCosTheta_t = std::sqrt(1.0f - sin2Theta_t);

        float r_par = (etaRatio * tempCosTheta_i - tempCosTheta_t) / (etaRatio * tempCosTheta_i + tempCosTheta_t);
        float r_perp = (etaRatio * tempCosTheta_t - tempCosTheta_i) / (etaRatio * tempCosTheta_t + tempCosTheta_i);
        *cosTheta_i = tempCosTheta_i;
        *cosTheta_t = tempCosTheta_t;

        return (r_par * r_par + r_perp * r_perp) / 2.0f;
    }

    static Vec3 refract(const Vec3& in, const Vec3& normal, float etaRatio, float cosTheta_i, float cosTheta_t) {
        return -in / etaRatio + (cosTheta_i / etaRatio - cosTheta_t) * normal;
    }

private:
    float indexOfRefraction;
};
