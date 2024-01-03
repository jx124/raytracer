#include "math/rng.h"

RNG::RNG(uint64_t seed) {
    state = seed + increment;
    (void)pcg32();
}

RNG::RNG(const RNG &other) : state(other.state) {}

RNG::RNG(RNG &&other) : state(other.state) {}

RNG &RNG::operator=(const RNG &other) {
    state = other.state;
    return *this;
}

RNG &RNG::operator=(RNG &&other) {
    state = other.state;
    return *this;
}

template<>
uint32_t RNG::uniform<uint32_t>() {
	return pcg32();
}

template<>
int32_t RNG::uniform<int32_t>() {
	uint32_t v = RNG::uniform<uint32_t>();
    if (v <= (uint32_t)std::numeric_limits<int32_t>::max()) {
        return int32_t(v);
	}
    return int32_t(v - std::numeric_limits<int32_t>::min()) + std::numeric_limits<int32_t>::min();
}

template<>
float RNG::uniform<float>() {
	return std::min<float>(oneMinusEpsilon, uniform<uint32_t>() * 0x1p-32f);
}

uint32_t RNG::rotr32(uint32_t x, unsigned r) {
    return x >> r | x << (-r & 31);
}

uint32_t RNG::pcg32() {
    uint64_t x = state;
    unsigned count = (unsigned)(x >> 59);

    state = x * multiplier + increment;
    x ^= x >> 18;
    return rotr32((uint32_t)(x >> 27), count);
}
