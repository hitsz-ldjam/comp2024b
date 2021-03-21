#include "model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>


bgfx::VertexLayout Vertex::get_layout() {
    bgfx::VertexLayout layout;
    layout.begin()
          .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
          .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
          .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
          .end();
    return layout;
}


Model::Model(const std::string& filename) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    // todo: play with material
    const auto flag = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str());
    if(!flag)
        throw std::runtime_error(err);

    for(const auto& shape : shapes) {
        // todo: play with shape.name

        for(const auto& index : shape.mesh.indices) {
            auto vertex = Vertex{
                {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                },
                {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                },
                {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
                }
            };
            vertices.emplace_back(vertex);
            indices.push_back(indices.size());
        }
    }

    const auto layout = Vertex::get_layout();
    vbh = bgfx::createVertexBuffer(bgfx::makeRef(vertices.data(), sizeof(Vertex) * vertices.size()), layout);
    ibh = bgfx::createIndexBuffer(bgfx::makeRef(indices.data(), sizeof(u16) * indices.size()));
}


Model::~Model() {
    bgfx::destroy(ibh);
    bgfx::destroy(vbh);
}
