#include "camera.hpp"

Camera::Camera(
    const point3& eye,
    const point3& target,
    const vec3& up,
    const float vfov,
    const float aspect_ratio
)
{
    const auto theta = degrees_to_radians(vfov);
    const auto h = std::tan(theta / 2.0f);
    const auto viewport_height = 2.0f * h;
    const auto viewport_width = aspect_ratio * viewport_height;

    const auto w = vec3::unit_vector(eye - target);
    const auto u = vec3::unit_vector(vec3::cross(up, w));
    const auto v = vec3::cross(w, u);

    m_eye = eye;
    m_horizontal = viewport_width * u;
    m_vertical = viewport_height * v;
    m_lower_left = m_eye - (m_horizontal / 2.0f) - (m_vertical / 2.0f) - w;
}