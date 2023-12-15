#include "sampler.h"

Vec3 sampleUniformSphere(Vec2 u) {
    float z = 1 - 2 * u.x;
    float r = std::sqrt(1 - z * z);
    float phi = 2 * pi * u.y;
    return {r * std::cos(phi), r * std::sin(phi), z};
}
