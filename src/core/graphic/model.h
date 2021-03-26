#pragma once

#include "../types.h"

#include <bgfx/bgfx.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <string>
#include <vector>
#include <optional>
#include <memory>


// not a component
// dirty code
struct MeshDataTuple {
    bgfx::VertexBufferHandle vbh;
    bgfx::IndexBufferHandle ibh;
    glm::vec4 diffuse;

    glm::mat4 transform;
};


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    static bgfx::VertexLayout get_layout();
};


// todo: load from asset manager
class Model final {
public:
    Model() = default;
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;
    Model(Model&& other) noexcept ;
    Model& operator=(Model&& other) noexcept ;
    ~Model();

    static std::optional<Model> load_from_file(const std::string& filename);
    static std::shared_ptr<Model> load_from_file_shared(const std::string& filename);

private:
    std::vector<Vertex> vertices;
    std::vector<u16> indices;

    // todo: move to MeshRenderer component
public:
    bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;

    std::vector<MeshDataTuple> mdt;
};
