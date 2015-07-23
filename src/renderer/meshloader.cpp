#include "meshloader.hpp"

#include <boost/iostreams/device/mapped_file.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>

#include <glm/gtx/io.hpp>

#include "iqm.h"

mesh_data load_mesh(const std::string& filename)
{
    using namespace boost::iostreams;

    mapped_file_source mesh_file("../meshes/" + filename + ".iqm");

    const iqmheader* ih = reinterpret_cast<const iqmheader*>(mesh_file.data());

    const iqmvertexarray* vaptr = reinterpret_cast<const iqmvertexarray*>(mesh_file.data() + ih->ofs_vertexarrays);

    const iqmvertexarray* posva = vaptr;
    const iqmvertexarray* uvva = vaptr + 1;
    const iqmvertexarray* normalva = vaptr + 2;

    mesh_data mesh;
    mesh.vertices.reserve(ih->num_vertexes);
    mesh.indices.reserve(ih->num_triangles);

    auto positions = reinterpret_cast<const glm::vec3*>(mesh_file.data() + posva->offset);
    auto normals = reinterpret_cast<const glm::vec3*>(mesh_file.data() + normalva->offset);
    auto texcoords = reinterpret_cast<const glm::vec2*>(mesh_file.data() + uvva->offset);

    for (unsigned int i = 0; i < ih->num_vertexes; i++) {
        auto pos = positions[i];
        auto nor = normals[i];
        auto tc = texcoords[i];

        mesh.vertices.emplace_back(pos);
        mesh.attributes.emplace_back(nor, tc);
    }

    auto triangles = reinterpret_cast<const unsigned int*>(mesh_file.data() + ih->ofs_triangles);
    for (unsigned int i = 0; i < ih->num_triangles * 3; i++) {
        auto t = triangles[i];
        mesh.indices.emplace_back(t);
    }

    return mesh;
}
