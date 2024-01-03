#pragma once

#include "utils.h"
#include "hittable.h"

class Quad : public Hittable {
public:
    Quad(const Vec3& Q, const Vec3& u, const Vec3& v, std::shared_ptr<Material> mat);

    AABB boundingBox() const override;
    bool hit(const Ray& ray, Interval rayT, HitRecord& rec) const override;

private:
    Vec3 Q, u, v; // Q is the "origin" and u/v are the two vectors defining the area of the quad.
    Vec3 normal;
    Vec3 w; // n / (n dot n)
    float D; // plane parameter
    AABB bbox;
    std::shared_ptr<Material> mat;
};