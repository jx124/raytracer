#pragma once

#include "hittable.h"

#include <memory>
#include <vector>

class Scene : public Hittable {
public:
    Scene() = default;
    Scene(std::shared_ptr<Hittable> object);

    void add(std::shared_ptr<Hittable> object);
    void clear();
    bool hit(const Ray& ray, Interval rayT, HitRecord& rec) const override;
    AABB boundingBox() const override;

    std::vector<std::shared_ptr<Hittable>> objects; // TODO: check if shared_ptr is required.

private:
    AABB bbox;
};