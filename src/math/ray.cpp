#include "math/ray.h"

Ray::Ray(Vec3 origin, Vec3 direction) : orig(origin), dir(direction) {}

Vec3 Ray::at(float t) const {
    return orig + t * dir;
}
