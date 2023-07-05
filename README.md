# Ray Tracer

My implementation of the "Ray Tracing in One Weekend" article by Peter Shirley.

![Example image](https://reecocho.github.io/assets/ray_tracer_header.png)

Some additional features I included are:

* SIMD acceleration for math.
* Multithreaded image rendering.
* BVH acceleration structure.
* Convenient command line interface.
* PNG image output.
* Faster random number generation (using [Xoroshiro128+](https://en.wikipedia.org/wiki/Xoroshiro128%2B)).

## External Libraries

[stb_image_write](https://github.com/nothings/stb/blob/master/stb_image_write.h) was used to perform PNG output.

[args](https://github.com/Taywee/args) was used for the command line interface.
