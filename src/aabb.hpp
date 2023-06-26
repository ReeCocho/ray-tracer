#pragma once

#include <algorithm>
#include "vec3.hpp"
#include "ray.hpp"

class aabb
{
public:

    inline aabb() {}
    inline aabb(const point3& minim, const point3& maxim) : minimum(minim), maximum(maxim) {}

    inline point3 min() const { return minimum; }
    inline point3 max() const { return maximum; }

    inline bool hit(const ray& r, float t_min, float t_max) const
    { 
        const auto invD = r.inv_direction();
        auto t1 = (minimum - r.origin()) * invD;
        auto t2 = (maximum - r.origin()) * invD;

        auto tmin1 = vec3::min(t1, t2);
        auto tmax1 = vec3::max(t1, t2);

        t_min = tmin1.e[0] > t_min ? tmin1.e[0] : t_min;
        t_min = tmin1.e[1] > t_min ? tmin1.e[1] : t_min;
        t_min = tmin1.e[2] > t_min ? tmin1.e[2] : t_min;

        t_max = tmax1.e[0] < t_max ? tmax1.e[0] : t_max;
        t_max = tmax1.e[1] < t_max ? tmax1.e[1] : t_max;
        t_max = tmax1.e[2] < t_max ? tmax1.e[2] : t_max;

        return t_max >= t_min && t_min < t_max;
    }

    inline static aabb surrounding_box(const aabb& box0, const aabb& box1) 
    {
        const point3 small(
            fmin(box0.min().x(), box1.min().x()),
            fmin(box0.min().y(), box1.min().y()),
            fmin(box0.min().z(), box1.min().z())
        );
        const point3 big(
            fmax(box0.max().x(), box1.max().x()),
            fmax(box0.max().y(), box1.max().y()),
            fmax(box0.max().z(), box1.max().z())
        );
        return aabb(small, big);
    }

public:

    point3 minimum;
    point3 maximum;
};