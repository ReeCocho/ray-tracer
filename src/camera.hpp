#pragma once

#include "vec3.hpp"
#include "ray.hpp"

class Camera
{
public:

    Camera(
        const point3& eye,
        const point3& target,
        const vec3& up,
        const float vfov,
        const float aspect_ratio
    );

    inline ray screen_to_world(const float u, const float v) const noexcept
    {
        return ray
        (
            m_eye, 
            m_lower_left + u*m_horizontal + v*m_vertical - m_eye
        );
    }

private:

    point3 m_eye;
    point3 m_lower_left;
    vec3 m_horizontal;
    vec3 m_vertical;
};