#pragma once

#include "utils.h"
#include "hittable.h"
#include "scene.h"

#include <algorithm>

class BVHNode : public Hittable {
public:
    BVHNode(const Scene& scene) : BVHNode(scene.objects, 0, scene.objects.size()) {}

    BVHNode(const std::vector<std::shared_ptr<Hittable>>& srcObjects, size_t start, size_t end) {
        auto objects = srcObjects;

        int axis = rng.uniform<int>(3);
        auto comparator = (axis == 0) ? boxXCompare :
                          (axis == 1) ? boxYCompare :
                          boxZCompare;

        size_t objectSpan = end - start;
        if (objectSpan == 1) {
            left = right = objects[start];
        } else if (objectSpan == 2) {
            if (comparator(objects[start], objects[start + 1])) {
                left = objects[start];
                right = objects[start + 1];
            } else {
                right = objects[start];
                left = objects[start + 1];
            }
        } else {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);

            size_t mid = start + objectSpan / 2;
            left = std::make_shared<BVHNode>(objects, start, mid);
            right = std::make_shared<BVHNode>(objects, mid, end);
        }

        bbox = AABB(left->boundingBox(), right->boundingBox());
    }

    bool hit(const Ray& ray, Interval rayT, HitRecord& rec) const override {
        if (!bbox.hit(ray, rayT)) {
            return false;
        }

        bool hitLeft = left->hit(ray, rayT, rec);
        bool hitRight = right->hit(ray, Interval(rayT.min, hitLeft ? rec.t : rayT.max), rec);

        return hitLeft || hitRight;
    }

    AABB boundingBox() const override { return bbox; }

    static bool boxCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axisIndex) {
        return a->boundingBox().axis(axisIndex).min < b->boundingBox().axis(axisIndex).min;
    }

    static bool boxXCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
        return boxCompare(a, b, 0);
    }

    static bool boxYCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
        return boxCompare(a, b, 1);
    }

    static bool boxZCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
        return boxCompare(a, b, 2);
    }

private:
    static inline RNG rng = {0};
    std::shared_ptr<Hittable> left; // TODO: check if unique_ptr can be used
    std::shared_ptr<Hittable> right;
    AABB bbox;
};