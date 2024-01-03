#include "scene.h"

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

AABB Scene::boundingBox() const {
    return bbox;
}