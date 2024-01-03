#pragma once

#include <limits>
#include <algorithm>

constexpr float infinity = std::numeric_limits<float>::infinity();

struct Interval {
    Interval();
    Interval(float min, float max);
    Interval(const Interval& a, const Interval& b);

    bool contains(float x) const;
    bool surrounds(float x) const;
    float size() const;
    Interval expand(float delta) const;
    
    float min, max;
    static const Interval empty;
    static const Interval universe;
};

static const Interval empty(infinity, -infinity);
static const Interval universe(-infinity, infinity);
