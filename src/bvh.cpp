#include <algorithm>
#include "bvh.hpp"

inline bool box_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis);
bool box_x_compare (const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);
bool box_y_compare (const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);
bool box_z_compare (const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);

BvhNode::BvhNode(
    const std::vector<std::shared_ptr<Hittable>>& src_objects,
    const size_t start,
    const size_t end
)
{
    auto objects = src_objects;

    int axis = random_int(0, 2);
    const auto comparator = (axis == 0) ? box_x_compare
                        :   (axis == 1) ? box_y_compare
                                        : box_z_compare;

    const size_t object_span = end - start;

    if (object_span == 1)
    {
        m_left = m_right = objects[start];
    }
    else if (object_span == 2)
    {
        if (comparator(objects[start], objects[start + 1]))
        {
            m_left = objects[start];
            m_right = objects[start + 1];
        }
        else
        {
            m_left = objects[start + 1];
            m_right = objects[start];
        }
    }
    else
    {
        std::sort(objects.begin() + start, objects.begin() + end, comparator);
        const auto mid = start + object_span/2;
        m_left = std::make_shared<BvhNode>(objects, start, mid);
        m_right = std::make_shared<BvhNode>(objects, mid, end);
    }

    aabb box_left, box_right;

    if (!m_left->bounding_box(box_left) || !m_right->bounding_box(box_right))
        throw std::runtime_error("no bounding box in BvhNode constructor");

    m_bounds = aabb::surrounding_box(box_left, box_right);
}

bool BvhNode::hit(const ray& r, const float t_min, const float t_max, HitRecord& rec) const
{
    if (!m_bounds.hit(r, t_min, t_max))
        return false;

    bool hit_left = m_left->hit(r, t_min, t_max, rec);
    bool hit_right = m_right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

    return hit_left || hit_right;
}



inline bool box_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis)
{
    aabb box_a;
    aabb box_b;

    if (!a->bounding_box(box_a) || !b->bounding_box(box_b))
        throw std::runtime_error("no bounding box in BvhNode constructor");

    return box_a.min().e[axis] < box_b.min().e[axis];
}

bool box_x_compare (const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
{
    return box_compare(a, b, 0);
}

bool box_y_compare (const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
{
    return box_compare(a, b, 1);
}

bool box_z_compare (const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
{
    return box_compare(a, b, 2);
}