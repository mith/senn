#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <boost/iostreams/device/mapped_file.hpp>
#include <unordered_map>

#include "dirwatcher.hpp"
#include "backend.hpp"
#include "primitives.hpp"
#include "buffer.hpp"

struct Mesh {
    VertexBuffer<Vertex> vertex_buffer;
    VertexBuffer<VertexAttribes> attribute_buffer;
    IndexBuffer<unsigned int> indice_buffer;
    VertexArray vao;
    DrawCommand draw_command;

    Mesh() = default;
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& o) = default;
    Mesh& operator=(Mesh&& o) = default;
};

class MeshLoader {
    const std::string mesh_dir;
    DirWatcher mesh_dir_watcher;

    std::unordered_map<std::string, Mesh> loaded_meshes;

    static Mesh load_mesh_from_file(const std::string& filename);

public:
    MeshLoader(const std::string& mesh_dir);
    ~MeshLoader() { stop(); };

    void update_meshes();
    void stop();
    void start();

    Mesh* load_mesh(const std::string& filename);
};
