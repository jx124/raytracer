#pragma once

#include "glm/vec3.hpp"

using Vec3 = glm::vec3;

struct Ray {
    Ray() = default;
    Ray(Vec3 origin, Vec3 direction);
    
    Vec3 at(float t) const;

    Vec3 orig;
    Vec3 dir;
};