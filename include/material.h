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
    virtual BSDFSample sampleBSDF(const Vec3& in, const Vec3& normal, Sampler* sampler) const = 0;
};

class Lambertian : public Material {
public:
    Lambertian(Vec3 color) : color(color) {};

    virtual BSDFSample sampleBSDF(const Vec3& wo, const Vec3& normal, Sampler* sampler) const override {
        (void) wo;
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

    virtual BSDFSample sampleBSDF(const Vec3& wo, const Vec3& normal, Sampler* sampler) const override {
        Vec3 in = -glm::normalize(wo);
        // float cosTheta_i = glm::dot(in, normal);
        float cosTheta_t;
        // float etaRatio = indexOfRefraction;

        // if (cosTheta_i < 0.0f) {
        //     etaRatio = 1.0f / etaRatio;
        //     cosTheta_i = -cosTheta_i;
        // }

        float R = fresnelReflectance(in, normal, indexOfRefraction, &cosTheta_t);
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
            BSDFCos = Vec3(T); // TODO: compensate for 1/eta^2 factor
            wi = refract(in, normal, indexOfRefraction, cosTheta_t);
            pdf = 1.0f;
        }

        return {BSDFCos, wi, pdf};
    }

    // Returns the probability of reflection.
    static float fresnelReflectance(const Vec3& in, const Vec3& normal, float etaRatio, float* cosTheta_t) {
        float cosTheta_i = glm::dot(in, normal);

        if (cosTheta_i < 0.0f) {
            etaRatio = 1.0f / etaRatio;
            cosTheta_i = -cosTheta_i;
        }

        float sin2Theta_i = std::max(0.0f, 1.0f - (cosTheta_i * cosTheta_i));
        float sin2Theta_t = sin2Theta_i / (etaRatio * etaRatio);
        if (sin2Theta_t >= 1.0f) {
            return 1.0f;
        }
        float tempCosTheta_t = std::sqrt(1.0f - sin2Theta_t);

        float r_par = (etaRatio * cosTheta_i - tempCosTheta_t) / (etaRatio * cosTheta_i + tempCosTheta_t);
        float r_perp = (etaRatio * tempCosTheta_t - cosTheta_i) / (etaRatio * tempCosTheta_t + cosTheta_i);
        *cosTheta_t = tempCosTheta_t;

        return (r_par * r_par + r_perp * r_perp) / 2.0f;
    }

    static Vec3 refract(const Vec3& in, Vec3 normal, float etaRatio, float cosTheta_t) {
        float cosTheta_i = glm::dot(in, normal);

        if (cosTheta_i < 0.0f) {
            etaRatio = 1.0f / etaRatio;
            cosTheta_i = -cosTheta_i;
            normal = -normal;
        }

        return -in / etaRatio + (cosTheta_i / etaRatio - cosTheta_t) * normal;
    }

private:
    float indexOfRefraction;
};
