# Naive Path Tracer

A CPU path tracer in C++ with BVH acceleration, Monte Carlo sampling, specular reflection, and refraction.

---

## Sample Images

![Refractive teapot with caustics](sample_images/teapot_1440p.png)
*Refractive glass teapot with caustics — 1440p*

![Specular dragon](sample_images/Dragon_32ssp_reflectance.png)
*Specular dragon — 32 samples per pixel*

![Shiny dragon](sample_images/shiny_dragon.png)
*Shiny dragon*

---

## Features

- **BVH acceleration** — spatial median split on alternating axes (composite pattern)
- **Möller–Trumbore intersection** — fast ray/triangle intersection
- **Area lights with stratified sampling** — soft shadows via jittered grid supersampling
- **Fresnel equations** — physically based specular reflection and refraction (Snell's law, total internal reflection)
- **Russian Roulette path termination** — unbiased stochastic depth cutoff driven by surface reflectance
- **Cosine-weighted hemisphere sampling** — importance sampling for diffuse indirect illumination
- **OpenMP parallelisation** — dynamic scheduling over scanlines

---

## Architecture

```
Object (abstract)
├── Sphere          — analytic ray/sphere intersection
├── Triangle        — Möller–Trumbore, Phong normal interpolation
├── Plane           — infinite plane
├── AABB            — axis-aligned bounding box
└── BVH             — recursive bounding volume hierarchy (leaf = triangle mesh)

Light (abstract)
└── AreaLight       — stratified sampling over a parallelogram emitter

Camera              — generates primary rays through a virtual image plane
Sampler             — thread-safe jittered random number generation
WaveFrontParser     — loads .obj meshes and builds a BVH
```

The path tracing kernel (`Lo` in `lambertian_reflection`) evaluates the rendering equation recursively, coupling diffuse and specular terms so caustics arise naturally.

---

## Build

Requires a C++11 compiler with OpenMP support (GCC or Clang recommended).

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
./CGProject
```

The output is written as a `.ppm` file in the working directory.

> **Note:** mesh data files (`.obj`) are not included in the repository. Place them under `data/` before running.

---

## Development Journal

See [DEVLOG.md](DEVLOG.md) for a chronological record of design decisions, dead ends, and lessons learned during development — from the first ray/sphere intersection in February 2020 through BVH construction, area lights, Russian Roulette, and Fresnel refraction.
