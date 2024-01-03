#pragma once

#include "utils.h"
#include "hittable.h"
#include "scene.h"
#include "rng.h"

#include <algorithm>

class BVHNode : public Hittable {
public:
    BVHNode(const Scene& scene);
    BVHNode(const std::vector<std::shared_ptr<Hittable>>& srcObjects, size_t start, size_t end);

    bool hit(const Ray& ray, Interval rayT, HitRecord& rec) const override;

    AABB boundingBox() const override;
    static bool boxCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axisIndex);
    static bool boxXCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);
    static bool boxYCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);
    static bool boxZCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);

private:
    static inline RNG rng = {0};
    std::shared_ptr<Hittable> left; // TODO: check if unique_ptr can be used
    std::shared_ptr<Hittable> right;
    AABB bbox;
};