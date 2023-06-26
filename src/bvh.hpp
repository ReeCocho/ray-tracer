#pragma once

#include <vector>
#include "hittable.hpp"

class BvhNode : public Hittable
{
public:

    inline BvhNode() {};

    BvhNode(
        const std::vector<std::shared_ptr<Hittable>>& objects,
        const size_t start,
        const size_t end
    );

    bool hit(const ray& r, const float t_min, const float t_max, HitRecord& rec) const override;

    inline bool bounding_box(aabb& output_box) const override
    {
        output_box = m_bounds;
        return true;
    }

private:

    std::shared_ptr<Hittable> m_left;
    std::shared_ptr<Hittable> m_right;
    aabb m_bounds;
};