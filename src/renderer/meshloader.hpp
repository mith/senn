#pragma once

#include <vector>
#include <string>
#include <boost/iostreams/device/mapped_file.hpp>

#include "primitives.hpp"

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<VertexAttribes> attributes;
    std::vector<unsigned int> indices;
};

MeshData load_mesh(const std::string& filename);
