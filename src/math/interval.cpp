#include "math/interval.h"

Interval::Interval() : min(+infinity), max(-infinity) {}

Interval::Interval(float min, float max) : min(min), max(max) {}

Interval::Interval(const Interval &a, const Interval &b) : min(std::min(a.min, b.min)), max(std::max(a.max, b.max)) {}

bool Interval::contains(float x) const {
    return min <= x && x <= max;
}

bool Interval::surrounds(float x) const {
    return min < x && x < max;
}

float Interval::size() const {
    return max - min;
}

Interval Interval::expand(float delta) const {
    return Interval(min - delta / 2, max + delta / 2);
}