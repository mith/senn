#include "sceneloader.hpp"

#include <yaml-cpp/yaml.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <GL/glew.h>
#define KTX_OPENGL 1
#include <ktx.h>

#include "meshloader.hpp"

SceneLoader::SceneLoader(const std::string& scene_dir, ShaderLoader* shader_loader, MeshLoader* mesh_loader)
    : scene_dir(scene_dir)
    , shader_loader(shader_loader)
    , mesh_loader(mesh_loader)
    , scene_dir_watcher(scene_dir)
{
    start();
}

void SceneLoader::update_scenes()
{
    if (scene_dir_watcher.dir_changed()) {
        for (auto & loaded_scene : loaded_scenes) {
            loaded_scene.second = std::move(load_from_file(scene_dir + "/" + loaded_scene.first + ".yaml"));
            std::cout << "scene " << loaded_scene.first << " reloaded" << std::endl;
        }
    }
}

Scene* SceneLoader::load_scene(const std::string& scene_name)
{
    auto it = loaded_scenes.find(scene_name);
    if (it != loaded_scenes.end()) {
        return &it->second;
    }

    auto scene = load_from_file(scene_dir + "/" + scene_name + ".yaml");

    auto added = loaded_scenes.emplace(std::make_pair(scene_name, std::move(scene)));
    
    return &added.first->second;
}

Scene SceneLoader::load_from_file(const std::string& filename)
{
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "Loading scene");
    auto scene_file = YAML::LoadFile(filename);

    Scene scene;

    for (const auto & it : scene_file["materials"]) {
        Material material;
        GLenum target;
        GLenum glerror;
        GLboolean is_mipmapped;
        KTX_error_code ktxerror;
        const std::string diffuse_filename = it.second["diffuse"].as<std::string>();
        ktxerror = ktxLoadTextureN(("textures/" + diffuse_filename + ".ktx").c_str(),
                &material.diffuse.name, &target, nullptr, &is_mipmapped, &glerror, 0, nullptr);
        scene.materials.emplace(it.first.as<std::string>(), std::move(material));
        GLint intfor;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &intfor);
        printf("internalformat: %x\n", intfor);

        std::cout << "loaded texture: " << diffuse_filename << std::endl;
    }


    for (auto n : scene_file["objects"]) {
        auto md = mesh_loader->load_mesh(n["mesh"].as<std::string>());

        scene.meshes.push_back(md);

        ObjectAttribes obj_atr;
        obj_atr.orientation = n["orientation"].as<glm::quat>();
        obj_atr.position = n["position"].as<glm::vec3>();
        obj_atr.scale = n["scale"].as<glm::vec3>();
        scene.objects_attributes.push_back(obj_atr);

        auto tex = &scene.materials[n["material"].as<std::string>()].diffuse;
        scene.mesh_textures.emplace_back(tex);
    }

    scene.shader = shader_loader->load_shader(ShaderDescriptor{"simple.vert", "simple.frag"});
    scene.directional_lights.emplace_back(shader_loader, glm::normalize(glm::vec3(1.0f, 1.0f, 0.5f)));
    scene.camera.size = {1680, 1050};
    scene.camera.fovy = 50.0f;
    glPopDebugGroup();

    return scene;
}


void SceneLoader::stop()
{
    scene_dir_watcher.stop();
}

void SceneLoader::start()
{
    scene_dir_watcher.start();
}

namespace YAML {
    template <>
        struct convert<glm::vec3> {
            static bool decode(const Node& node, glm::vec3& v)
            {
                if (node.size() != 3) {
                    return false;
                }

                v.x = node[0].as<float>();
                v.y = node[1].as<float>();
                v.z = node[2].as<float>();
                return true;
            }
        };

    template <>
        struct convert<glm::quat> {
            static bool decode(const Node& node, glm::quat& q)
            {
                if (node.size() != 4) {
                    return false;
                }

                q.x = node[0].as<float>();
                q.y = node[1].as<float>();
                q.z = node[2].as<float>();
                q.w = node[3].as<float>();
                return true;
            }
        };
}
