#pragma once

#include "utils.h"
#include "material.h"
#include "aabb.h"

struct HitRecord {
    Vec3 point;
    Vec3 normal;
    float t;
    bool frontFace;
    std::shared_ptr<Material> mat;

    void setFaceNormal(const Ray& ray, const Vec3& outwardNormal);
};

class Hittable {
public:
    virtual ~Hittable() = default;
    
    virtual bool hit(const Ray& ray, Interval rayT, HitRecord& rec) const = 0;
    virtual AABB boundingBox() const = 0;
};