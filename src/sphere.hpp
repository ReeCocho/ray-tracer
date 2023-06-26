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
        // Equation for sphere of radius r at center C is:
        // (x - Cx)^2 + (y - Cy)^2 + (z - Cz)^2 = r^2
        //
        // Observe that this is really a dot product. Let P = (x, y, z):
        // (P - C) dot (P - C) = r^2
        //
        // We want to see if this equation is satisfied for any points along the ray R, so we can
        // make P a function of t, as in P(t) = A + tb, where A is the ray origin and b is the ray
        // direction. So, we get:
        //
        // (P(t) - C) dot (P(t) - C) = r^2
        // (A + tb - C) dot (A + tb - C) = r^2
        // (t^2)(b dot b) + [2tb dot (A - C)] + [(A - C) dot (A - C)] - r^2 = 0
        //
        // This is really just a quadratic equation which we can solve.

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