#include "meshloader.hpp"

#include <boost/iostreams/device/mapped_file.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>

#include <glm/gtx/io.hpp>

#include "iqm.h"

MeshLoader::MeshLoader(const std::string& mesh_dir)
    : mesh_dir(mesh_dir)
    , mesh_dir_watcher(mesh_dir)
{
    start();
}

void MeshLoader::update_meshes()
{
    if (mesh_dir_watcher.dir_changed()) {
        for (auto& loaded_mesh : loaded_meshes) {
            glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, 
                    ("reloading mesh: " + loaded_mesh.first).c_str());
            auto mesh = load_mesh_from_file(loaded_mesh.first);
            loaded_mesh.second = std::move(mesh);
            std::cout << "mesh " << loaded_mesh.first << " reloaded" << std::endl;
            glPopDebugGroup();
        }
    }
}

Mesh* MeshLoader::load_mesh(const std::string& mesh_name)
{
    auto it = loaded_meshes.find(mesh_name);
    if (it != loaded_meshes.end()) {
        return &it->second;
    }

    auto mesh = load_mesh_from_file(mesh_name);
    auto added = loaded_meshes.emplace(mesh_name, std::move(mesh));
    return &added.first->second;
}

Mesh MeshLoader::load_mesh_from_file(const std::string& filename)
{
    using namespace boost::iostreams;
    mapped_file_source mesh_file("../meshes/" + filename + ".iqm");

    const iqmheader* ih = reinterpret_cast<const iqmheader*>(mesh_file.data());

    const iqmvertexarray* vaptr = reinterpret_cast<const iqmvertexarray*>(mesh_file.data() + ih->ofs_vertexarrays);

    const iqmvertexarray* posva = vaptr;
    const iqmvertexarray* uvva = vaptr + 1;
    const iqmvertexarray* normalva = vaptr + 2;

    auto positions = reinterpret_cast<const glm::vec3*>(mesh_file.data() + posva->offset);
    auto normals = reinterpret_cast<const glm::vec3*>(mesh_file.data() + normalva->offset);
    auto texcoords = reinterpret_cast<const glm::vec2*>(mesh_file.data() + uvva->offset);

    std::vector<VertexAttribes> attributes;

    for (unsigned int i = 0; i < ih->num_vertexes; i++) {
        auto nor = normals[i];
        auto tc = texcoords[i];

        attributes.emplace_back(nor, tc);
    }

    auto indices = reinterpret_cast<const unsigned int*>(mesh_file.data() + ih->ofs_triangles);

    Mesh mesh;

    mesh.draw_command.index_count = ih->num_triangles;
    mesh.draw_command.base_index = 0;
    mesh.draw_command.base_vertex = 0;

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, ("creating buffers for mesh: " + filename).c_str());

    glCreateBuffers(1, &mesh.vertex_buffer.name);
    glObjectLabel(GL_BUFFER, mesh.vertex_buffer.name, -1, "vertice buffer");
    glNamedBufferStorage(mesh.vertex_buffer.name, sizeof(Vertex) * ih->num_vertexes,
        positions, 0);

    glCreateBuffers(1, &mesh.attribute_buffer.name);
    glObjectLabel(GL_BUFFER, mesh.attribute_buffer.name, -1, "attributes buffer");
    glNamedBufferStorage(mesh.attribute_buffer.name, sizeof(VertexAttribes) * attributes.size(),
        attributes.data(), 0);

    glCreateBuffers(1, &mesh.indice_buffer.name);
    glObjectLabel(GL_BUFFER, mesh.indice_buffer.name, -1, "indice buffer");
    glNamedBufferStorage(mesh.indice_buffer.name, sizeof(unsigned int) * ih->num_triangles * 3,
        indices, 0);

    glCreateVertexArrays(1, &mesh.vao.name);
    glBindVertexArray(mesh.vao.name);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer.name);
    glEnableVertexArrayAttrib(mesh.vao.name, 0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.attribute_buffer.name);
    glEnableVertexArrayAttrib(mesh.vao.name, 1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribes), nullptr);
    glEnableVertexArrayAttrib(mesh.vao.name, 2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribes),
        (void*)offsetof(VertexAttribes, texcoord));

    glPopDebugGroup();

    std::cout << "returning loaded mesh" << std::endl;
    return mesh;
}

void MeshLoader::stop()
{
    mesh_dir_watcher.stop();
}

void MeshLoader::start()
{
    mesh_dir_watcher.start();
}
