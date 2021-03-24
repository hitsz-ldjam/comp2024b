#include "model.h"
#include <stdexcept>
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

Model::Model(Model&& other) noexcept
    : vertices(std::move(other.vertices)),
      indices(std::move(other.indices)),
      vbh(other.vbh),
      ibh(other.ibh) {
    other.vbh = BGFX_INVALID_HANDLE;
    other.ibh = BGFX_INVALID_HANDLE;
}

Model& Model::operator=(Model&& other) noexcept {
    using std::swap;
    swap(vertices, other.vertices);
    swap(indices, other.indices);
    swap(vbh, other.vbh);
    swap(ibh, other.ibh);
    return *this;
}

Model::~Model() {
    if(bgfx::isValid(ibh))
        bgfx::destroy(ibh);
    if(bgfx::isValid(vbh))
        bgfx::destroy(vbh);
    ibh = BGFX_INVALID_HANDLE;
    vbh = BGFX_INVALID_HANDLE;
}

std::optional<Model> Model::load_from_file(const std::string& filename) {
    using std::nullopt;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;
    std::vector<Vertex> vertices;
    std::vector<u16> indices;
    bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;

    // todo: play with material
    const auto flag = tinyobj::LoadObj(&attrib, &shapes, &materials,&warn, &err, filename.c_str());
    if(!flag) {
        return nullopt;
    }

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

    Model result;
    result.vertices = std::move(vertices);
    result.indices = std::move(indices);
    result.vbh = vbh;
    result.ibh = ibh;
    return result;
}

std::shared_ptr<Model> Model::load_from_file_shared(const std::string& filename) {
    std::optional<Model> result = load_from_file(filename);
    if(result.has_value()) {
        return std::make_shared<Model>(std::move(result.value()));
    } else {
        return nullptr;
    }
}
