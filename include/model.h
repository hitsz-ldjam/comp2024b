#pragma once

#include "types.h"

#include <bgfx/bgfx.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <vector>


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    static bgfx::VertexLayout get_layout();
};


// todo: load from asset manager
class Model final {
public:
    Model(const std::string& filename);
    Model(const Model&) = delete;
    //Model(Model&&) = default;
    ~Model();

private:
    std::vector<Vertex> vertices;
    std::vector<u16> indices;

    // todo: move to MeshRenderer component
public:
    bgfx::VertexBufferHandle vbh;
    bgfx::IndexBufferHandle ibh;
};
