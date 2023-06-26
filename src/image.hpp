#pragma once

#include <vector>
#include <algorithm>
#include "vec3.hpp"

struct Pixel 
{
    float r;
    float g;
    float b;
    float a;
};

class Image
{
public:

    Image(const size_t width, const size_t height);

    inline size_t width() const noexcept { return m_width; }
    inline size_t height() const noexcept { return m_height; }
    inline float aspect_ratio() const noexcept { return float(m_width) / float(m_height); }

    inline void set_pixel(const size_t x, const size_t y, const color& value) noexcept
    {
        if (x >= m_width || y >= m_height) return;
        m_pixels[(((m_height - 1) - y) * m_width) + x] = Pixel 
        {
            value.x(),
            value.y(),
            value.z(),
            1.0f
        };
    }

    inline Pixel get_pixel(const size_t x, const size_t y) const noexcept
    {
        if (x >= m_width || y >= m_height) return Pixel { 0, 0, 0, 0};
        return m_pixels[(((m_height - 1) - y) * m_width) + x];
    }

    void average(const Image& other);

    void save(const char* path) const;

private:

    std::vector<Pixel> m_pixels;
    size_t m_width;
    size_t m_height;
};