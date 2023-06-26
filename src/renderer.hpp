#pragma once

#include <cstdint>
#include <vector>

#include "image.hpp"
#include "world.hpp"
#include "camera.hpp"

struct RenderArgs
{
    size_t thread_count;
    size_t samples;
    size_t width;
    size_t height;
};

class Renderer
{
public:

    Renderer(RenderArgs args);

    /**
     * Renders the `world` from the perspective of the `camera` using the render settings sent
     * to the constructor of the renderer. The final image is returned.
     */
    Image render(const Camera& camera, const World& world);

private:

    /**
     * Renders a set of images with samples distributed across multiple threads. These images can
     * be averaged together to produce a final image.
     */
    std::vector<Image> render_images(const Camera& camera, const World& world);

    /**
     * Takes every image from `src` and returns the average image. This process is destructive to
     * the images in `src`.
     */
    Image average_images(std::vector<Image> src);

    RenderArgs m_args;
};