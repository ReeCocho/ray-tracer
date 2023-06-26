#pragma once

#include "vec3.hpp"

class ray 
{
public:
    inline ray() {}
    inline ray(const point3& origin, const vec3& direction)
        : orig(origin), dir(direction), inv_dir(1.0f / direction)
    {}

    inline point3 origin() const noexcept { return orig; }
    inline vec3 direction() const noexcept { return dir; }
    inline vec3 inv_direction() const noexcept { return inv_dir; }

    inline point3 at(float t) const noexcept
    {
        return orig + t*dir;
    }

public:
    point3 orig;
    vec3 dir;
    vec3 inv_dir;
};