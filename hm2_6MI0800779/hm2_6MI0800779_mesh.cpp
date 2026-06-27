#include "hm2_6MI0800779_mesh.hpp"
#include <fstream>
#include <stdexcept>

class SimpleFileWriter : public mc::Writer {
    std::ostream& os;
public:
    SimpleFileWriter(std::ostream& out) : os(out) {}
    int operator()(const char* data, size_t length) override {
        os.write(data, length);
        return !os.good();
    }
};

Mesh::Mesh() : mesh_ptr(mc::MESH_NONE) {}

Mesh::Mesh(const mc::SDF& sdf, int nx, int ny, int nz, const mc::aabb& bounds) : mesh_ptr(mc::MESH_NONE) {
    int rc = mc::mesh_from_sdf(sdf, nx, ny, nz, bounds, &mesh_ptr);
    if (rc != MC_OK) {
        throw std::runtime_error("Runtime error: Invalid input!");
    }
}

Mesh::~Mesh() {
    mc::mesh_destroy(&mesh_ptr);    
}

Mesh::Mesh(Mesh&& other) noexcept : mesh_ptr(other.mesh_ptr) {
    other.mesh_ptr = mc::MESH_NONE;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        mc::mesh_destroy(&mesh_ptr);
        mesh_ptr = other.mesh_ptr;
        other.mesh_ptr = mc::MESH_NONE;
    }
    return *this;
}

Mesh& Mesh::write_to_gltf(const char* filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Runtime error: Cannot open file!");
    }
    write_to_gltf(file);
    return *this;
}

Mesh& Mesh::write_to_gltf(std::ostream& out) {
    if (mesh_ptr == mc::MESH_NONE) {
        throw std::runtime_error("Runtime error: No Mesh found!");
    }
    SimpleFileWriter writer(out);
    int result = mc::mesh_write_to_gltf(&mesh_ptr, writer);
    if (result != MC_OK) {
        throw std::runtime_error("Runtime error: Couldn't write output!");
    }
    std::cout << "Mesh successfully saved to output stream!" << std::endl;
    return *this;
}