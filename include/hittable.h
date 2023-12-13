#pragma once

#include "ray.h"
#include "glm/glm.hpp"

struct HitRecord {
    Vec3 point;
    Vec3 normal;
    float t;
    bool frontFace;

    void setFaceNormal(const Ray& ray, const Vec3& outwardNormal) {
        // Sets normal to point outwards.
        // Note: outwardNormal assumed to have unit length.

        frontFace = glm::dot(ray.dir, outwardNormal) < 0.0f;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }
};

class Hittable {
public:
    virtual ~Hittable() = default;
    virtual bool hit(const Ray& ray, float tMin, float tMax, HitRecord& rec) const = 0;
};