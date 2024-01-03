#pragma once

#include <cstdint>
#include <cmath>
#include <limits>
#include <type_traits>

constexpr float oneMinusEpsilon = 0x1.fffffep-1;

class RNG {
public:
	RNG(uint64_t seed);
	RNG(const RNG& other);
	RNG(RNG&& other);

	RNG& operator=(const RNG& other);
	RNG& operator=(RNG&& other);

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
	uint32_t rotr32(uint32_t x, unsigned r);
	uint32_t pcg32();

    uint64_t state = 0x4d595df4d0f33173;
	uint64_t const multiplier = 6364136223846793005u;
	uint64_t const increment = 1442695040888963407u;
};

template<>
uint32_t RNG::uniform<uint32_t>();

template<>
int32_t RNG::uniform<int32_t>();

template<>
float RNG::uniform<float>();
