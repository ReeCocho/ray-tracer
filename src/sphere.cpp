#include "sphere.hpp"

Sphere::Sphere(const point3& center, const float radius, const MaterialId mat) :
    m_mat(mat),
    m_center(center),
    m_radius(radius),
    m_sqr_radius(radius * radius)
{}