#include "light.h"

Light::Light(const Vec3 &Q, const Vec3 &u, const Vec3 &v, std::shared_ptr<Material> mat) : Quad(Q, u, v, mat) {
    area = glm::length(glm::cross(u, v));
}

LightSample Light::sampleLight(const Vec3& origin, Sampler *sampler) const {
    Vec2 sample = sampler->get2D();
    Vec3 intersection = Q + sample.x * u + sample.y * v;
    Vec3 diff = intersection - origin;

    float distSquared = glm::dot(diff, diff);
    float cosine = std::abs(glm::dot(glm::normalize(diff), normal));

    return {mat->Le(diff, normal), intersection, distSquared / (area * cosine)};
}
