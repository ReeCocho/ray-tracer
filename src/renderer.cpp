#include <cassert>
#include <thread>
#include "renderer.hpp"

static void render_thread(unsigned int, Image*, const Camera*, const World*, const size_t);
static void average_images_thread(Image*, Image*);

Renderer::Renderer(RenderArgs args) :
    m_args(args)
{
    assert(m_args.width > 0 && m_args.height > 0);
    assert(m_args.thread_count > 0 && (m_args.thread_count & (m_args.thread_count - 1)) == 0);
}

Image Renderer::render(const Camera& camera, const World& world)
{
    auto images = render_images(camera, world);
    return average_images(std::move(images));
}

std::vector<Image> Renderer::render_images(const Camera& camera, const World& world)
{
    // Threads dispatch based on image samples
    std::vector<Image> images = {};
    std::vector<std::thread> threads = {};
    const size_t samples_per_thread = m_args.samples / m_args.thread_count;

    // Create images
    for (size_t i = 0; i < m_args.thread_count; i++)
        images.push_back(Image(m_args.width, m_args.height));

    // Create render threads
    for (size_t i = 0; i < m_args.thread_count; i++)
    {
        std::thread thread(
            render_thread, 
            i,
            &images[i],
            &camera,
            &world,
            samples_per_thread + (
                i == m_args.thread_count - 1
                ? m_args.samples % m_args.thread_count
                : 0
            )
        );
        threads.push_back(std::move(thread));
    }

    // Wait for threads to complete
    for (auto& thread : threads)
        thread.join();

    return std::move(images);
}

Image Renderer::average_images(std::vector<Image> src)
{
    std::vector<std::thread> threads = {};

    // We average together the images by powers of 2. The final image will be at images[0]
    size_t offset = 2;
    size_t count = m_args.thread_count / 2;
    while (count != 0)
    {
        // Spawn threads to average the images
        for (size_t i = 0; i < m_args.thread_count; i += offset)
        {
            std::thread thread(
                average_images_thread,
                &src[i],
                &src[i + (offset / 2)]
            );
            threads.push_back(std::move(thread));
        }

        // Wait for the averaging to finish
        for (auto& thread : threads)
            thread.join();
        threads.clear();

        offset *= 2;
        count /= 2;
    }

    return std::move(src[0]);
}

void render_thread(
    unsigned int thread_id,
    Image* image, 
    const Camera* camera, 
    const World* world,
    const size_t samples_per_pixel
)
{
    seed_random_float(thread_id);

    const size_t width = image->width();
    const size_t height = image->height();

    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            auto pixel_color = color(0, 0, 0);

            for (size_t i = 0; i < samples_per_pixel; i++)
            {
                const auto u = (float(x) + random_float()) / float(width - 1);
                const auto v = (float(y) + random_float()) / float(height - 1);
                const auto r = camera->screen_to_world(u, v);
                pixel_color += world->ray_color(r);
            }

            // Gamma correct
            const auto scale = 1.0f / float(samples_per_pixel);
            pixel_color.e[0] = std::sqrt(scale * pixel_color.e[0]);
            pixel_color.e[1] = std::sqrt(scale * pixel_color.e[1]);
            pixel_color.e[2] = std::sqrt(scale * pixel_color.e[2]);
            image->set_pixel(x, y, pixel_color);
        }
    }
}

void average_images_thread(Image* dst, Image* src)
{
    dst->average(*src);
}