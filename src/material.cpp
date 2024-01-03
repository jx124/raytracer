#include "material.h"

// Emitted radiance
Vec3 Material::Le(const Vec3 &wo, const Vec3 &normal) const {
    (void) wo;
    (void) normal;
    return Vec3(0.0f);
}

Lambertian::Lambertian(Vec3 color, Vec3 emissionColor) : color(color), emissionColor(emissionColor) {}

BSDFSample Lambertian::sampleBSDF(const Vec3 &wo, const Vec3 &normal, Sampler *sampler, bool frontFace) const {
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

Vec3 Lambertian::Le(const Vec3 &wo, const Vec3 &normal) const {
    (void) wo;
    (void) normal; // For future use in directional lighting?
    return emissionColor;
}

Metal::Metal(Vec3 color, float fuzz) : color(color), fuzz(fuzz) {}

BSDFSample Metal::sampleBSDF(const Vec3 &wo, const Vec3 &normal, Sampler *sampler, bool frontFace) const {
    (void) frontFace;
    Vec3 BSDFCos = color;
    Vec3 wi = reflect(wo, normal) + fuzz * sampleUniformSphere(sampler->get2DPixel());
    float pdf = 1.0f;

    return {BSDFCos, wi, pdf};
}

Vec3 Metal::reflect(const Vec3 &in, const Vec3 &normal)
{
    return in - 2 * glm::dot(in, normal) * normal;
}

Dielectric::Dielectric(float indexOfRefraction) : indexOfRefraction(indexOfRefraction) {}

BSDFSample Dielectric::sampleBSDF(const Vec3 &wo, const Vec3 &normal, Sampler *sampler, bool frontFace) const {
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

float Dielectric::fresnelReflectance(const Vec3 &in, const Vec3 &normal, 
                                     float etaRatio, float *cosTheta_i, float *cosTheta_t) {
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

Vec3 Dielectric::refract(const Vec3 &in, const Vec3 &normal, float etaRatio, float cosTheta_i, float cosTheta_t) {
    return -in / etaRatio + (cosTheta_i / etaRatio - cosTheta_t) * normal;
}