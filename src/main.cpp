#include <iostream>
#include <thread>
#include <vector>
#include <cassert>
#include <chrono>

#include "args.hpp"
#include "common.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "world.hpp"
#include "sphere.hpp"
#include "material.hpp"
#include "renderer.hpp"

static World construct_default_world();

int main(int argc, const char** argv)
{
    args::ArgumentParser p("parser");
    args::HelpFlag help(p, "help", "Display this help menu", {'h', "help"});
    args::ValueFlag<int> samples(p, "samples", "Number of samples taken per pixel. Must be non-zero.", { "samples" }, 64);
    args::ValueFlag<int> threads(p, "threads", "Number of threads to use when rendering the image. Must be a power of 2.", { "threads" }, 4);
    args::ValueFlag<int> width(p, "width", "Width in pixels of the image. Must be non-zero.", { "width" }, 560);
    args::ValueFlag<int> height(p, "height", "Height in pixels of the image. Must be non-zero.", { "height" }, 315);
    args::CompletionFlag completion(p, {"complete"});

    try
    {
        p.ParseCLI(argc, argv);
    }
    
    catch (const args::Completion &e)
    {
        std::cout << e.what();
        return 0;
    }
    catch (const args::Help&)
    {
        std::cout << p;
        return 0;
    }
    catch (const args::ParseError& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << p;
        return 1;
    }

    // Verify arguments
    if (samples.Get() <= 0)
    {
        std::cerr << "Sample count must be a positive integer.";
        return 1;
    }

    if (width.Get() <= 0 || height.Get() <= 0)
    {
        std::cerr << "Image width and height must both be positive integers.";
        return 1;
    }

    if (threads.Get() <= 0 || (threads.Get() & (threads.Get() - 1)) != 0)
    {
        std::cerr << "Thread count must be a non-zero power of 2.";
        return 1;
    }

    RenderArgs args;
    args.width = width.Get();
    args.height = height.Get();
    args.thread_count = threads.Get();
    args.samples = samples.Get();

    std::cout << "Rendering scene width...\n"
        << "Image dimensions: (" << args.width << ", " << args.height << ")\n"
        << "Thread count: " << args.thread_count << "\n"
        << "Sample count: " << args.samples << std::endl;

    auto camera = Camera(
        point3(13, 2, 3),
        point3(0, 0, 0),
        vec3(0, 1, 0),
        20.0f,
        float(width.Get()) / float(height.Get())
    );
    
    std::cout << "Constructing a default world..." << std::endl;
    const auto world = construct_default_world();
    
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    Renderer renderer(args);

    std::cout << "Beginning render..." << std::endl;
    const auto image = renderer.render(camera, world);

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
 
    std::cout << "Render time: " << elapsed_seconds.count() << "s" << std::endl;

    std::cout << "Saving image './output.png'..." << std::endl;
    image.save("./output.png");
    std::cout << "Complete." << std::endl;

    return 0;
}

World construct_default_world()
{    
    seed_random_float(500);
    auto world = World();

    const auto material_ground = world.add_material(Lambertian(color(0.8f, 0.8f, 0.8f)));
    world.add_object(Sphere(point3(0, -1000, -1), 1000, material_ground));

    for (int x = -11; x < 11; x++)
    {
        for (int y = -11; y < 11; y++)
        {
            const auto choose_mat = random_float();
            const point3 center(x + 0.9f * random_float(), 0.2f, y + 0.9f * random_float());

            if ((center - point3(4, 0.2f, 0)).length() > 0.9f)
            {
                MaterialId sphere_material;

                if (choose_mat < 0.9f)
                {
                    // Diffuse
                    const auto albedo = color::random() * color::random();
                    sphere_material = world.add_material(Lambertian(albedo));
                }
                else if (choose_mat < 0.95f)
                {
                    // Metal
                    const auto albedo = color::random(0.5f, 1.0f);
                    const auto fuzz = random_float(0.0f, 0.5f);
                    sphere_material = world.add_material(Metal(albedo, fuzz));
                }
                else
                {
                    // Glass
                    sphere_material = world.add_material(Dielectric(1.5f));
                }

                world.add_object(Sphere(center, 0.2f, sphere_material));
            }
            
        }
    }

    auto material_center = world.add_material(Lambertian(color(0.1f, 0.2f, 0.5f)));
    auto material_left   = world.add_material(Dielectric(1.5f));
    auto material_right  = world.add_material(Metal(color(0.8f, 0.6f, 0.2f), 0.0f));

    world.add_object(Sphere(point3( 0.0f, -100.5f, -1.0f), 100.0f, material_ground));

    world.add_object(Sphere(point3( .0f, 1.0f, 0.0f),  1.0f, material_center));
    world.add_object(Sphere(point3(-4.0f, 1.0f, 0.0f), 1.0f, material_left));
    world.add_object(Sphere(point3(-4.0f, 1.0f, 0.0f), -0.95f, material_left));
    world.add_object(Sphere(point3(4.0f, 1.0f, 0.0f), 1.0f, material_right));

    world.compute_bvh();

    return std::move(world);
}