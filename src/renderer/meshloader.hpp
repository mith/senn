#pragma once

#include <vector>
#include <string>
#include <boost/iostreams/device/mapped_file.hpp>

#include "primitives.hpp"

struct mesh_data {
    std::vector<vertex> vertices;
    std::vector<vertex_attributes> attributes;
    std::vector<unsigned int> indices;
};

mesh_data load_mesh(const std::string & filename);
