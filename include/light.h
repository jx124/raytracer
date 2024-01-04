#pragma once

#include "utils.h"
#include "material.h"
#include "hittables/quad.h"

struct LightSample {
    Vec3 L;
    Vec3 intersection;
    float pdf = 0;
};

class Light : public Quad {
public:
    Light(const Vec3 &Q, const Vec3 &u, const Vec3 &v, std::shared_ptr<Material> mat);

    LightSample sampleLight(const Vec3& origin, Sampler *sampler) const;

private:
    float area;
};