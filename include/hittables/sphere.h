#pragma once

#include "hittable.h"

class Sphere : public Hittable {
public:
    Sphere(Vec3 center, float radius, std::shared_ptr<Material> material);

    bool hit(const Ray& ray, Interval rayT, HitRecord& rec) const override;
    AABB boundingBox() const override;

private:
    Vec3 center;
    float radius;
    std::shared_ptr<Material> material;
    AABB bbox;
};