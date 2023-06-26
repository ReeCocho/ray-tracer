#pragma once

#include "ray.hpp"
#include "vec3.hpp"

struct HitRecord;

using MaterialId = size_t;

class Material
{
public:

    virtual bool scatter(
        const ray& r_in, 
        const HitRecord& rec, 
        color& attenuation, 
        ray& scattered
    ) const = 0;
};



class Lambertian : public Material
{
public:

    Lambertian(const color& albedo);

    bool scatter(
        const ray& r_in, 
        const HitRecord& rec, 
        color& attenuation, 
        ray& scattered
    ) const override;

private:

    color m_albedo;
};

class Metal : public Material
{
public:

    Metal(const color& albedo, const float roughness);

    bool scatter(
        const ray& r_in, 
        const HitRecord& rec, 
        color& attenuation, 
        ray& scattered
    ) const override;

private:

    color m_albedo;
    float m_roughness;
};

class Dielectric : public Material
{
public:

    Dielectric(const float index_of_refraction);

    bool scatter(
        const ray& r_in, 
        const HitRecord& rec, 
        color& attenuation, 
        ray& scattered
    ) const override;

private:

    float m_index_of_refraction;
};