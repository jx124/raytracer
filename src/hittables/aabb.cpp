#include "hittables/aabb.h"

AABB::AABB(Interval x, Interval y, Interval z) : x(x), y(y), z(z) {}

AABB::AABB(const Vec3& a, const Vec3& b) {
    x = Interval(std::min(a.x, b.x), std::max(a.x, b.x));
    y = Interval(std::min(a.y, b.y), std::max(a.y, b.y));
    z = Interval(std::min(a.z, b.z), std::max(a.z, b.z));
}

AABB::AABB(const AABB &box0, const AABB &box1) {
    x = Interval(box0.x, box1.x);
    y = Interval(box0.y, box1.y);
    z = Interval(box0.z, box1.z);
}

const Interval& AABB::axis(int n) const {
    if (n == 0) {
        return x;
    } else if (n == 1) {
        return y;
    } else {
        return z;
    }
}

bool AABB::hit(const Ray &ray, Interval rayT) const {
    for (int a = 0; a < 3; a++) {
        float invD = 1.0f / ray.dir[a];
        float origin = ray.orig[a];

        float t0 = (axis(a).min - origin) * invD;
        float t1 = (axis(a).max - origin) * invD;

        if (invD < 0.0f) {
            std::swap(t0, t1);
        }

        if (t0 > rayT.min) {
            rayT.min = t0;
        }
        if (t1 < rayT.max) {
            rayT.max = t1;
        }

        if (rayT.max <= rayT.min) {
            return false;
        }
    }
    return true;
}

AABB AABB::pad() {
    constexpr float delta = 0.0001f;
    Interval newX = (x.size() >= delta) ? x : x.expand(delta);
    Interval newY = (y.size() >= delta) ? y : y.expand(delta);
    Interval newZ = (z.size() >= delta) ? z : z.expand(delta);

    return AABB(newX, newY, newZ);
}