#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <boost/iostreams/device/mapped_file.hpp>
#include <unordered_map>

#include "dirwatcher.hpp"
#include "primitives.hpp"

struct Mesh {
    Buffer<Vertex> vertex_buffer;
    Buffer<VertexAttribes> attribute_buffer;
    Buffer<unsigned int> indice_buffer;
    VertexArray vao;
    DrawCommand draw_command;

    Mesh()
        : vertex_buffer()
        , attribute_buffer()
        , indice_buffer()
        , vao()
        , draw_command()
    {}
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
