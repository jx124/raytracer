#include "hittables/quad.h"

Quad::Quad(const Vec3 &Q, const Vec3 &u, const Vec3 &v, std::shared_ptr<Material> mat) : Q(Q), u(u), v(v), mat(mat) { 
    bbox = AABB(Q, Q + u + v).pad();

    Vec3 n = glm::cross(u, v);
    normal = glm::normalize(n);
    w = n / (glm::dot(n, n));
    D = glm::dot(normal, Q);
}

AABB Quad::boundingBox() const {
    return bbox;
}

bool Quad::hit(const Ray &ray, Interval rayT, HitRecord &rec) const {
    float denom = glm::dot(normal, ray.dir);

    // parallel to plane
    if (std::abs(denom) < 1e-8f) {
        return false;
    }

    // hit outside 
    float t = (D - glm::dot(normal, ray.orig)) / denom;
    if (!rayT.contains(t)) {
        return false;
    }

    // check if hit point within quad using plane coordinates
    Vec3 intersection = ray.at(t);
    Vec3 p = intersection - Q;
    float alpha = glm::dot(w, glm::cross(p, v));
    float beta = glm::dot(w, glm::cross(u, p));

    if (alpha < 0 || alpha > 1 || beta < 0 || beta > 1) {
        return false;
    }

    rec.t = t;
    rec.point = intersection;
    rec.mat = mat;
    rec.setFaceNormal(ray, normal);

    return true;
}
