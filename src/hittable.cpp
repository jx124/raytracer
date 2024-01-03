#include "hittable.h"

void HitRecord::setFaceNormal(const Ray &ray, const Vec3 &outwardNormal) {
    // Sets normal to be on the same face as hit ray.
    // Note: outwardNormal assumed to have unit length.

    frontFace = glm::dot(ray.dir, outwardNormal) < 0.0f;
    normal = frontFace ? outwardNormal : -outwardNormal;
}