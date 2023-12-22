#pragma once

#include "hittable.h"

class Sphere : public Hittable {
public:
    Sphere(Vec3 center, float radius, std::shared_ptr<Material> material) 
        : center(center), radius(radius), material(material) {}

    bool hit(const Ray& ray, Interval rayT, HitRecord& rec) const override {
        Vec3 oc = ray.orig - center;
        float a = glm::dot(ray.dir, ray.dir);
        float halfB = glm::dot(oc, ray.dir);
        float c = glm::dot(oc, oc) - radius * radius;
        float discriminant = halfB * halfB - a * c;

        if (discriminant < 0) {
            return false;
        } 
        float sqrtDisc = std::sqrt(discriminant);
        
        // Find nearest root in acceptable range
        float root = (-halfB - sqrtDisc) / a;
        if (!rayT.surrounds(root)) {
            root = (-halfB + sqrtDisc) / a;
            if (!rayT.surrounds(root)) {
                return false;
            }
        }

        rec.t = root;
        rec.point = ray.at(root);
        Vec3 normal = (rec.point - center) / radius;
        // rec.setFaceNormal(ray, normal);
        rec.normal = normal;
        rec.mat = material;

        return true;
    }

private:
    Vec3 center;
    float radius;
    std::shared_ptr<Material> material;
};