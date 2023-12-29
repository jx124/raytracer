#pragma once

#include <limits>
#include <algorithm>

constexpr float infinity = std::numeric_limits<float>::infinity();

struct Interval {
    Interval() : min(+infinity), max(-infinity) {}
    Interval(float min, float max) : min(min), max(max) {}
    Interval(const Interval& a, const Interval& b) : min(std::min(a.min, b.min)), max(std::max(a.max, b.max)) {}

    bool contains(float x) const { return min <= x && x <= max; }
    bool surrounds(float x) const { return min < x && x < max; }
    float size() const { return max - min; }
    Interval expand(float delta) { return Interval(min - delta / 2, max + delta / 2); }
    
    float min, max;
    static const Interval empty;
    static const Interval universe;
};

static const Interval empty(infinity, -infinity);
static const Interval universe(-infinity, infinity);
