#pragma once

#include <limits>

constexpr float infinity = std::numeric_limits<float>::infinity();

struct Interval {
    Interval() : min(+infinity), max(-infinity) {}
    Interval(float min, float max) : min(min), max(max) {}

    bool contains(float x) { return min <= x && x <= max; }
    bool surrounds(float x) { return min < x && x < max; }
    
    float min, max;
    static const Interval empty;
    static const Interval universe;
};

static const Interval empty(infinity, -infinity);
static const Interval universe(-infinity, infinity);
