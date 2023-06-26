#pragma once

// Modified from the article

#include <cmath>
#include <iostream>
#include "common.hpp"

#ifdef _MSC_VER 
    #include <xmmintrin.h>
#endif

class alignas(16) vec3 
{
public:

    inline vec3(float e0, float e1, float e2) : e{e0, e1, e2, 0} {}

    inline float x() const { return e[0]; }
    inline float y() const { return e[1]; }
    inline float z() const { return e[2]; }

#if !ENABLE_SIMD
    
    inline vec3() : e{0,0,0,0} {}
    
    inline static vec3 max(const vec3& a, const vec3& b)
    {
        vec3 res;
        res.e[0] = std::max(a.e[0], b.e[0]);
        res.e[1] = std::max(a.e[1], b.e[1]);
        res.e[2] = std::max(a.e[2], b.e[2]);
        return res;
    }

    inline static vec3 min(const vec3& a, const vec3& b)
    {
        vec3 res;
        res.e[0] = std::min(a.e[0], b.e[0]);
        res.e[1] = std::min(a.e[1], b.e[1]);
        res.e[2] = std::min(a.e[2], b.e[2]);
        return res;
    }

    inline vec3 operator-() const {  
        return vec3(-e[0], -e[1], -e[2]); 
    }

    inline vec3& operator+=(const vec3 &v) 
    {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    inline vec3& operator*=(const float t) 
    {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    inline vec3& operator*=(const vec3 &v) 
    {
        e[0] *= v.e[0];
        e[1] *= v.e[1];
        e[2] *= v.e[2];
        return *this;
    }

    inline static float dot(const vec3 &u, const vec3 &v)
    {
        return u.e[0] * v.e[0]
            + u.e[1] * v.e[1]
            + u.e[2] * v.e[2];
    }

    inline static vec3 cross(const vec3 &u, const vec3 &v) 
    {
        return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                    u.e[2] * v.e[0] - u.e[0] * v.e[2],
                    u.e[0] * v.e[1] - u.e[1] * v.e[0]);
    }

    inline float length_squared() const 
    {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }

    inline static vec3 unit_vector(const vec3 v) 
    {
        const auto l = v.length();
        return vec3(v.e[0] / l, v.e[1] / l, v.e[2] / l);
    }

#elif defined(_MSC_VER)

    inline vec3() : v(_mm_set_ps(0, 0, 0, 0)) {}

    inline static vec3 max(const vec3& a, const vec3& b)
    {
        vec3 res;
        res.v = _mm_max_ps(a.v, b.v);
        return res;
    }

    inline static vec3 min(const vec3& a, const vec3& b)
    {
        vec3 res;
        res.v = _mm_min_ps(a.v, b.v);
        return res;
    }

    inline vec3 operator-() const { 
        vec3 res;
        const __m128 self = _mm_load_ps(e);
        const __m128 neg = _mm_set_ps(0.0f, -1.0f, -1.0f, -1.0f);
        res.v = _mm_mul_ps(self, neg);
        return res; 
    }

    inline vec3& operator+=(const vec3 &v) 
    {
        const __m128 self = _mm_load_ps(e);
        const __m128 other = _mm_load_ps(v.e);
        this->v = _mm_add_ps(self, other);
        return *this;
    }

    inline vec3& operator*=(const float t) 
    {
        const __m128 self = _mm_load_ps(e);
        const __m128 ts = _mm_set_ps(0, t, t, t);
        this->v = _mm_mul_ps(self, ts);
        return *this;
    }

    inline vec3& operator*=(const vec3& u) 
    {
        const __m128 self = _mm_load_ps(e);
        const __m128 us = _mm_load_ps(u.e);
        this->v = _mm_mul_ps(self, us);
        return *this;
    }

    inline static float dot(const vec3 &u, const vec3 &v)
    {
        alignas(16) float out[4];
        const __m128 v0 = _mm_mul_ps(u.v, v.v);
        _mm_store_ps(out, v0);
        return out[0] + out[1] + out[2];
    }

    inline static vec3 cross(const vec3 &u, const vec3 &v) 
    {
        vec3 out;
        const __m128 vec0 = _mm_load_ps(u.e);
        const __m128 vec1 = _mm_load_ps(v.e);
        const __m128 tmp0 = _mm_shuffle_ps(vec0, vec0, _MM_SHUFFLE(3,0,2,1));
        const __m128 tmp1 = _mm_shuffle_ps(vec1, vec1, _MM_SHUFFLE(3,1,0,2));
        const __m128 tmp2 = _mm_mul_ps(tmp0, vec1);
        const __m128 tmp3 = _mm_mul_ps(tmp0, tmp1);
        const __m128 tmp4 = _mm_shuffle_ps(tmp2, tmp2, _MM_SHUFFLE(3,0,2,1));
        out.v = _mm_sub_ps(tmp3,tmp4);
        return out;
    }

    inline float length_squared() const 
    {
        return vec3::dot(*this, *this);
    }

    inline static vec3 unit_vector(vec3 v) 
    {
        const auto l = v.length();
        return v *= (1/l);
    }

#endif

    inline float operator[](int i) const { return e[i]; }
    inline float& operator[](int i) { return e[i]; }

    inline vec3& operator/=(const float t) 
    {
        return *this *= 1/t;
    }

    inline float length() const 
    {
        return std::sqrt(length_squared());
    }

    inline bool near_zero() const 
    {
        // Return true if the vector is close to zero in all dimensions.
        constexpr float s = 1e-6f;
        return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
    }

    inline static vec3 reflect(const vec3& v, const vec3& n);

    inline static vec3 refract(const vec3& uv, const vec3& n, const float i_o_r);

    inline static vec3 random()
    {
        return vec3(random_float(), random_float(), random_float());
    }

    inline static vec3 random(const float min, const float max)
    {
        return vec3(random_float(min, max), random_float(min, max), random_float(min, max));
    }

    inline static vec3 random_in_unit_cube()
    {
        return vec3::random(-1.0f, 1.0f);
    }

    inline static vec3 random_in_unit_sphere()
    {
        while (true)
        {
            auto p = random_in_unit_cube();
            if (p.length_squared() >= 1) continue;
            return p;
        }
    }

    inline static vec3 random_unit_vector()
    {
        return vec3::unit_vector(random_in_unit_sphere());
    }

public:

#if !ENABLE_SIMD
    float e[4];
#elif defined(_MSC_VER) 
    union
    {
        float e[4];
        __m128 v;
    };
#endif
};

inline std::ostream& operator<<(std::ostream &out, const vec3 &v) 
{
    return out << '(' << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2] << ')';
}

inline vec3 operator+(const vec3 &u, const vec3 &v) 
{
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

#if !ENABLE_SIMD
    inline vec3 operator-(const vec3 &u, const vec3 &v) 
    {
        return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
    }

    inline vec3 operator*(const vec3 &u, const vec3 &v) 
    {
        return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
    }

    inline vec3 operator*(float t, const vec3 &v) 
    {
        return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
    }

    inline vec3 operator/(float t, const vec3 &v) 
    {
        return vec3(t/v.e[0], t/v.e[1], t/v.e[2]);
    }
#elif defined(_MSC_VER) 
    inline vec3 operator-(const vec3 &u, const vec3 &v) 
    {
        vec3 out;
        const __m128 us = _mm_load_ps(u.e);
        const __m128 vs = _mm_load_ps(v.e);
        out.v = _mm_sub_ps(us, vs);
        return out;
    }

    inline vec3 operator*(const vec3 &u, const vec3 &v) 
    {
        vec3 out;
        const __m128 us = _mm_load_ps(u.e);
        const __m128 vs = _mm_load_ps(v.e);
        out.v = _mm_mul_ps(us, vs);
        return out;
    }

    inline vec3 operator*(float t, const vec3 &v) 
    {
        vec3 out;
        const __m128 us = _mm_set_ps(0, t, t, t);
        const __m128 vs = _mm_load_ps(v.e);
        out.v = _mm_mul_ps(us, vs);
        return out;
    }

    inline vec3 operator/(float t, const vec3 &v) 
    {
        vec3 out;
        const __m128 us = _mm_set_ps(0, t, t, t);
        const __m128 vs = _mm_load_ps(v.e);
        out.v = _mm_div_ps(us, vs);
        return out;
    }
#endif

inline vec3 operator*(const vec3 &v, float t) 
{
    return t * v;
}

inline vec3 operator/(vec3 v, float t) 
{
    return (1/t) * v;
}

inline vec3 vec3::reflect(const vec3& v, const vec3& n)
{
    return v - 2.0f * vec3::dot(v, n) * n;
}

inline vec3 vec3::refract(const vec3& uv, const vec3& n, const float etai_over_etat)
{
    const auto cos_theta = std::fmin(vec3::dot(-uv, n), 1.0f);
    const vec3 r_out_perp = etai_over_etat * (uv + cos_theta*n);
    const vec3 r_out_parallel = -std::sqrt(std::fabs(1.0f - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

// Type aliases for vec3
using point3 = vec3;   // 3D point
using color = vec3;    // RGB color