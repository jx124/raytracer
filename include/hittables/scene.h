#pragma once

#include "utils.h"
#include "hittable.h"
#include "camera.h"
#include "sphere.h"
#include "quad.h"
#include "light.h"
#include "math/sampler.h"

#include <memory>
#include <vector>

class Scene : public Hittable {
public:
    Scene() = default;
    Scene(std::shared_ptr<Hittable> object);

    void add(std::shared_ptr<Hittable> object);
    void clear();
    bool hit(const Ray& ray, Interval rayT, HitRecord& rec) const override;
    Vec3 backgroundColor(const Vec3& direction, int type) const;
    AABB boundingBox() const override;
    static Scene generateScene(Sampler* sampler, Camera& camera, int index);

    std::vector<std::shared_ptr<Hittable>> objects; // TODO: check if shared_ptr is required.
    std::vector<std::shared_ptr<Light>> lights;

private:
    static Scene randomSpheres(Sampler* sampler, Camera& camera);
    static Scene quads(Camera& camera);
    static Scene cornellBox(Camera& camera);
    
    AABB bbox;
};