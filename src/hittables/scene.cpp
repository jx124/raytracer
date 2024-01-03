#include "hittables/scene.h"

Scene::Scene(std::shared_ptr<Hittable> object) {
    add(object);
}

void Scene::add(std::shared_ptr<Hittable> object) { 
    objects.push_back(object); 
    bbox = AABB(bbox, object->boundingBox());
}

void Scene::clear() {
    objects.clear();
}

bool Scene::hit(const Ray &ray, Interval rayT, HitRecord &rec) const {
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

Vec3 Scene::backgroundColor(const Vec3 &direction, int type) const {
    switch (type) {
    case 1:
        // black
        return Vec3(0.0f);
    case 2: default:
        // sky blue
        Vec3 unitDirection = glm::normalize(direction);
        float a = 0.5 * (unitDirection.y + 1.0f);
        return (1.0f - a) * Vec3(1.0f) + a * Vec3(0.5f, 0.7f, 1.0f);
    }
}

AABB Scene::boundingBox() const {
    return bbox;
}