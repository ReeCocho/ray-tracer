cmake_minimum_required(VERSION 3.0.0)
project(ray-tracer)

add_executable(ray-tracer-prog 
    src/stb_image_write.h    
    src/stb_image_write.cpp
    src/args.hpp

    src/main.cpp
    src/common.cpp
    src/common.hpp
    src/image.cpp
    src/image.hpp
    src/world.cpp
    src/world.hpp
    src/renderer.cpp
    src/renderer.hpp
    src/camera.cpp
    src/camera.hpp
    src/sphere.cpp
    src/sphere.hpp
    src/vec3.hpp
    src/aabb.hpp
    src/bvh.cpp
    src/bvh.hpp
    src/ray.hpp
    src/hittable.hpp
    src/material.cpp
    src/material.hpp
)

set_property(TARGET ray-tracer-prog PROPERTY CXX_STANDARD 17)
