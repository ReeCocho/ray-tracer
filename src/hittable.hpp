#pragma once

#include <memory>
#include "ray.hpp"
#include "material.hpp"
#include "aabb.hpp"

struct HitRecord
{
    MaterialId mat;
    point3 p;
    vec3 normal;
    float t;
    bool front_face;

    inline void set_face_normal(const ray& r, const vec3& outward_normal) 
    {
        front_face = vec3::dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable
{
public:

    virtual bool hit(const ray& r, const float t_min, const float t_max, HitRecord& rec) const = 0;
    virtual bool bounding_box(aabb& output_box) const = 0;
};