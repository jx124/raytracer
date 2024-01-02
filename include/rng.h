#pragma once

#include <cstdint>
#include <cmath>
#include <limits>

constexpr float oneMinusEpsilon = 0x1.fffffep-1;

class RNG {
public:
	RNG(uint64_t seed) {
		state = seed + increment;
		(void)pcg32();
	}

	RNG(const RNG& other) : state(other.state) {};
	RNG(RNG&& other) : state(other.state) {};

	RNG& operator=(const RNG& other) {
		state = other.state;
		return *this;
	};

	RNG& operator=(RNG&& other) {
		state = other.state;
		return *this;
	};

	template <typename T>
	T uniform();

	template <typename T>
	typename std::enable_if_t<std::is_integral_v<T>, T> uniform(T b) {
		T threshold = (~b + 1u) % b;
		while (true) {
			T r = uniform<T>();
			if (r >= threshold)
				return r % b;
		}
	}

private:
	uint32_t rotr32(uint32_t x, unsigned r) {
		return x >> r | x << (-r & 31);
	}

	uint32_t pcg32() {
		uint64_t x = state;
		unsigned count = (unsigned)(x >> 59);

		state = x * multiplier + increment;
		x ^= x >> 18;
		return rotr32((uint32_t)(x >> 27), count);
	}

    uint64_t state = 0x4d595df4d0f33173;
	uint64_t const multiplier = 6364136223846793005u;
	uint64_t const increment = 1442695040888963407u;
};

template<>
inline uint32_t RNG::uniform<uint32_t>() {
	return pcg32();
}

template<>
inline int32_t RNG::uniform<int32_t>() {
	uint32_t v = uniform<uint32_t>();
    if (v <= (uint32_t)std::numeric_limits<int32_t>::max()) {
        return int32_t(v);
	}
    return int32_t(v - std::numeric_limits<int32_t>::min()) + std::numeric_limits<int32_t>::min();
}

template<>
inline float RNG::uniform<float>() {
	return std::min<float>(oneMinusEpsilon, uniform<uint32_t>() * 0x1p-32f);
}