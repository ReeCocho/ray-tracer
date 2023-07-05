#pragma once

#define ENABLE_SIMD 1

#include <random>

#if ENABLE_SIMD
    #include <immintrin.h>
#endif

constexpr float PI = 3.14159265f;

inline float degrees_to_radians(float degrees) 
{
    return degrees * PI / 180.0f;
}

union RandomGenState
{
    alignas(32) uint64_t seed[4];
#if ENABLE_SIMD
    __m256i seed_v;
#endif
};

extern thread_local RandomGenState g_random_state;

static inline uint64_t rotl(const uint64_t x, int k) 
{
	return (x << k) | (x >> (64 - k));
}

inline void seed_random_float(unsigned int seed)
{
    static std::uniform_int_distribution<uint64_t> distribution(0, std::numeric_limits<uint64_t>::max());
    static std::mt19937 generator(seed);
    g_random_state.seed[0] = distribution(generator);
    g_random_state.seed[1] = distribution(generator);
    g_random_state.seed[2] = distribution(generator);
    g_random_state.seed[3] = distribution(generator);
}

inline float random_float() 
{
    const uint64_t result = g_random_state.seed[0] + g_random_state.seed[3];

	const uint64_t t = g_random_state.seed[1] << 17;

#if ENABLE_SIMD
    alignas(32) const uint64_t shifted[4] = 
    { 
        g_random_state.seed[2], 
        g_random_state.seed[3], 
        g_random_state.seed[1], 
        g_random_state.seed[0] 
    };
    
    const __m256i shiftedv = _mm256_load_si256((const __m256i*)shifted);
    g_random_state.seed_v = _mm256_xor_si256(g_random_state.seed_v, shiftedv);
#else
    g_random_state.seed[2] ^= g_random_state.seed[0];
    g_random_state.seed[3] ^= g_random_state.seed[1];
    g_random_state.seed[1] ^= g_random_state.seed[2];
    g_random_state.seed[0] ^= g_random_state.seed[3];
#endif

	g_random_state.seed[2] ^= t;
	g_random_state.seed[3] = rotl(g_random_state.seed[3], 45);

	return (*(uint32_t*)&result >> 8) * 0x1.0p-24f;
}

inline float random_float(const float min, const float max) 
{
    return min + (max - min) * random_float();
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_float((float)min, (float)max + 1.0f));
}