#include <limits>
#include "world.hpp"

World::World() :
    m_objects(),
    m_bvh_root(nullptr)
{}

bool World::hit(const ray& r, const float t_min, const float t_max, HitRecord& record) const
{
    return m_bvh_root == nullptr ? false : m_bvh_root->hit(r, t_min, t_max, record);

    /*
    HitRecord hit_record;
    bool hit_any = false;
    auto closest = t_max;

    for (const auto& obj : m_objects)
    {
        if (obj->hit(r, t_min, closest, hit_record))
        {
            hit_any = true;
            closest = hit_record.t;
            record = hit_record;
        }
    }

    return hit_any;
    */
}

void World::compute_bvh()
{
    m_bvh_root = std::make_unique<BvhNode>(m_objects, 0, m_objects.size());
}

color World::ray_color(const ray& r) const
{
    constexpr float T_MIN = 0.001f;
    constexpr float T_MAX = std::numeric_limits<float>::max();
    constexpr size_t MAX_BOUNCES = 16;

    HitRecord hit_record;
    color output_color = color(1, 1, 1);
    ray ray_dir = r;
    
    for (size_t depth = 0; depth < MAX_BOUNCES; depth++)
    {
        if (hit(ray_dir, T_MIN, T_MAX, hit_record))
        {
            ray scattered;
            color attenuation;

            if (m_materials[hit_record.mat]->scatter(ray_dir, hit_record, attenuation, scattered))
            {
                output_color *= attenuation;
                ray_dir = scattered;
            }
            else
            {
                break;
            }
        }
        else
        {
            const vec3 unit_dir = vec3::unit_vector(ray_dir.direction());
            const auto t = 0.5f * (unit_dir.y() + 1.0f);
            output_color *= (1.0f - t) * color(1.0f, 1.0f, 1.0f) + t*color(0.5f, 0.7f, 1.0f);
            return output_color;
        }
    }

    return color(0, 0, 0);
}
