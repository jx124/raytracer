#include "bvh_node.h"

BVHNode::BVHNode(const Scene &scene) : BVHNode(scene.objects, 0, scene.objects.size()) {}

BVHNode::BVHNode(const std::vector<std::shared_ptr<Hittable>> &srcObjects, size_t start, size_t end) {
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

bool BVHNode::hit(const Ray &ray, Interval rayT, HitRecord &rec) const {
    if (!bbox.hit(ray, rayT)) {
        return false;
    }

    bool hitLeft = left->hit(ray, rayT, rec);
    bool hitRight = right->hit(ray, Interval(rayT.min, hitLeft ? rec.t : rayT.max), rec);

    return hitLeft || hitRight;
}

AABB BVHNode::boundingBox() const {
    return bbox;
}

bool BVHNode::boxCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axisIndex) {
    return a->boundingBox().axis(axisIndex).min < b->boundingBox().axis(axisIndex).min;
}

bool BVHNode::boxXCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
    return boxCompare(a, b, 0);
}

bool BVHNode::boxYCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
    return boxCompare(a, b, 1);
}

bool BVHNode::boxZCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
    return boxCompare(a, b, 2);
}