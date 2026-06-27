#ifndef MESH_HPP
#define MESH_HPP

#include "marching_cubes.hpp"
#include <iostream>

class Mesh {
    mc::Mesh mesh_ptr;

public:
    Mesh();

    Mesh(const mc::SDF& sdf, int nx, int ny, int nz, const mc::aabb& bounds);

    ~Mesh();

    Mesh(Mesh&& other) noexcept;

    Mesh& operator=(Mesh&& other) noexcept;

    Mesh(const Mesh&) = delete;

    Mesh& operator=(const Mesh&) = delete;

    Mesh& write_to_gltf(const char* filename);

    Mesh& write_to_gltf(std::ostream& out);
};

#endif