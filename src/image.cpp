#include <cassert>
#include "image.hpp"
#include "stb_image_write.h"

Image::Image(const size_t width, const size_t height) :
    m_width(width),
    m_height(height),
    m_pixels()
{
    m_pixels.resize(m_width * m_height);
}

void Image::average(const Image& other)
{
    assert(m_width == other.width() && m_height == other.height());

    for (size_t y = 0; y < m_height; y++)
    {
        for (size_t x = 0; x < m_width; x++)
        {
            const size_t idx = (((m_height - 1) - y) * m_width) + x;
            m_pixels[idx].r = (m_pixels[idx].r + other.m_pixels[idx].r) / 2.0f;
            m_pixels[idx].g = (m_pixels[idx].g + other.m_pixels[idx].g) / 2.0f;
            m_pixels[idx].b = (m_pixels[idx].b + other.m_pixels[idx].b) / 2.0f;
        }
    }
}

void Image::save(const char* path) const
{
    assert(path != nullptr);

    std::vector<uint8_t> bytes = {};
    bytes.reserve(4 * m_width * m_height);

    for (const auto& pixel : m_pixels)
    {
        bytes.push_back(static_cast<uint8_t>(std::fmax(0.0, std::fmin(pixel.r * 255.999, 255.0))));
        bytes.push_back(static_cast<uint8_t>(std::fmax(0.0, std::fmin(pixel.g * 255.999, 255.0))));
        bytes.push_back(static_cast<uint8_t>(std::fmax(0.0, std::fmin(pixel.b * 255.999, 255.0))));
        bytes.push_back(static_cast<uint8_t>(std::fmax(0.0, std::fmin(pixel.a * 255.999, 255.0))));
    }

    stbi_write_png(path, (int)m_width, (int)m_height, 4, bytes.data(), m_width * 4);
}