#pragma once

#include "vec3.hpp"
#include "ray.hpp"
#include "hittable.hpp"

class Sphere : public Hittable
{
public:

    Sphere(const point3& center, const float radius, const MaterialId mat);

    inline float get_radius() const noexcept { return m_radius; }

    inline point3 get_center() const noexcept { return m_center; }

    bool hit(const ray& r, const float t_min, const float t_max, HitRecord& rec) const override
    {
        const auto oc = r.origin() - m_center;
        const auto a = r.direction().length_squared();
        const auto half_b = vec3::dot(
            oc, 
            r.direction()
        );
        const auto c = oc.length_squared() - m_sqr_radius;
        const auto discriminant = half_b*half_b - a*c;

        if (discriminant < 0) return false;

        const auto sqrtd = std::sqrt(discriminant);

        auto root = (-half_b - sqrtd) / a;
        if (root < t_min || t_max < root)
        {
            root = (-half_b + sqrtd) / a;
            if (root < t_min || t_max < root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        rec.mat = m_mat;
        const vec3 outward_normal = (rec.p - m_center) / m_radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }

    bool bounding_box(aabb& output_box) const override 
    {
        output_box = aabb(
            m_center - vec3(m_radius, m_radius, m_radius),
            m_center + vec3(m_radius, m_radius, m_radius)
        );
        return true;
    }

private:

    const MaterialId m_mat;
    const point3 m_center;
    const float m_radius;
    const float m_sqr_radius;
};