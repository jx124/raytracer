#pragma once

#include "hittable.h"

#include <memory>
#include <vector>

class Scene : public Hittable {
public:
    Scene() = default;
    Scene(std::shared_ptr<Hittable> object) { add(object); }

    void add(std::shared_ptr<Hittable> object) { 
        objects.push_back(object); 
        bbox = AABB(bbox, object->boundingBox());
    }

    void clear() { objects.clear(); }

    bool hit(const Ray& ray, Interval rayT, HitRecord& rec) const override {
        HitRecord tempRec;
        bool hitAnything = false;
        float closest = rayT.max;

        for (const auto &object : objects) {
            if (object->hit(ray, Interval(rayT.min, closest), tempRec)) {
                hitAnything = true;
                closest = tempRec.t;
                rec = tempRec;
            }
        }

        return hitAnything;
    }

    AABB boundingBox() const override { return bbox; }

    std::vector<std::shared_ptr<Hittable>> objects; // TODO: check if shared_ptr is required.
private:
    AABB bbox;
};