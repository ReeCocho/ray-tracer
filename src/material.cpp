#include "material.hpp"
#include "hittable.hpp"

Lambertian::Lambertian(const color& albedo) : m_albedo(albedo)
{}

bool Lambertian::scatter(
    const ray& r_in, 
    const HitRecord& rec, 
    color& attenuation, 
    ray& scattered
) const
{
    auto scatter_dir = rec.normal + vec3::random_unit_vector();

    // Catch bad scatter dir
    if (scatter_dir.near_zero())
        scatter_dir = rec.normal;

    scattered = ray(rec.p, scatter_dir);
    attenuation = m_albedo;
    return true;
}



Metal::Metal(const color& albedo, const float roughness) : 
    m_albedo(albedo),
    m_roughness(roughness)
{}

bool Metal::scatter(
    const ray& r_in, 
    const HitRecord& rec, 
    color& attenuation, 
    ray& scattered
) const
{
    const auto reflected = vec3::reflect(vec3::unit_vector(r_in.direction()), rec.normal);
    scattered = ray(rec.p, reflected + (m_roughness * vec3::random_in_unit_sphere()));
    attenuation = m_albedo;
    return vec3::dot(scattered.direction(), rec.normal) > 0;
}



Dielectric::Dielectric(const float index_of_refraction) :
    m_index_of_refraction(index_of_refraction)
{}

static float reflectance(const float cosine, const float ref_idx)
{
    // Schlick approximation
    float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
    r0 = r0 * r0;
    return r0 + (1.0f - r0) * std::pow(1.0f - cosine, 5.0f);
}

bool Dielectric::scatter(
    const ray& r_in, 
    const HitRecord& rec, 
    color& attenuation, 
    ray& scattered
) const
{
    attenuation = color(1.0f, 1.0f, 1.0f);
    const float refraction_ratio = 
        rec.front_face ? (1.0f / m_index_of_refraction) : m_index_of_refraction;
    
    const vec3 unit_dir = vec3::unit_vector(r_in.direction());
    const float cos_theta = std::fmin(vec3::dot(-unit_dir, rec.normal), 1.0f);
    const float sin_theta = std::sqrt(1.0f - cos_theta*cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0f;
    vec3 dir = (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_float()) ?
        vec3::reflect(unit_dir, rec.normal) :
        vec3::refract(unit_dir, rec.normal, refraction_ratio);

    scattered = ray(rec.p, dir);
    return true;
}