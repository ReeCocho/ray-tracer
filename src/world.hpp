#pragma once

#include <vector>
#include <memory>
#include "hittable.hpp"
#include "material.hpp"
#include "vec3.hpp"
#include "ray.hpp"
#include "bvh.hpp"

class World
{
public:

    World();

    template<typename T> inline void add_object(const T& obj)
    {
        m_objects.push_back(std::make_shared<T>(obj));
    }

    template<typename T> inline MaterialId add_material(const T& mat)
    {
        m_materials.push_back(std::make_shared<T>(mat));
        return m_materials.size() - 1;
    }

    bool hit(const ray& r, const float t_min, const float t_max, HitRecord& record) const;

    void compute_bvh();

    color ray_color(const ray& r) const;

private:

    std::unique_ptr<BvhNode> m_bvh_root;
    std::vector<std::shared_ptr<Hittable>> m_objects;
    std::vector<std::shared_ptr<Material>> m_materials;
};