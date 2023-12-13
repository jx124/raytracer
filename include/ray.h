#pragma once

#include "glm/vec3.hpp"

using Vec3 = glm::vec3;

struct Ray {
    Ray() = default;
    Ray(Vec3 origin, Vec3 direction) : orig(origin), dir(direction) {}
    
    Vec3 at(float t) const { return orig + t * dir; }

    Vec3 orig;
    Vec3 dir;
};