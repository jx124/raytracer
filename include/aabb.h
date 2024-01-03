#pragma once

#include "utils.h"
#include "interval.h"

class AABB {
public:
    AABB() = default;
    AABB(Interval x, Interval y, Interval z);
    AABB(const Vec3& a, const Vec3& b);
    AABB(const AABB& box0, const AABB& box1);

    const Interval& axis(int n) const;
    bool hit(const Ray& ray, Interval rayT) const;
    AABB pad();
    
private:
    Interval x, y, z;
};