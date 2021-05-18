#include "model.h"
#include "binary_reader.h"
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <stdexcept>
#include <fstream>

// todo: add support for .obj
//#include <tinyobjloader/tiny_obj_loader.h>
#include <OpenFBX/ofbx.h>

bgfx::VertexLayout Vertex::get_layout() {
    bgfx::VertexLayout layout;
    layout.begin()
          .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
          .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
          .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
          .end();
    return layout;
}

Model::Model(Model&& other) noexcept : vertices(std::move(other.vertices)),
                                       indices(std::move(other.indices)),
                                       vbh(other.vbh),
                                       ibh(other.ibh),
                                       mdt(std::move(other.mdt)),
                                       aabb(std::move(other.aabb)) {
    other.vbh = BGFX_INVALID_HANDLE;
    other.ibh = BGFX_INVALID_HANDLE;
}

Model& Model::operator=(Model&& other) noexcept {
    using std::swap;
    swap(vertices, other.vertices);
    swap(indices, other.indices);
    swap(vbh, other.vbh);
    swap(ibh, other.ibh);
    swap(mdt, other.mdt);
    swap(aabb, other.aabb);
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


auto load_fbx_model(const std::string& filename) {
    size_t filesize = 0;
    auto alloc = [&](auto size) { return std::make_unique<ofbx::u8[]>(filesize = size); };
    auto get = [](const auto& ptr) { return ptr.get(); };
    auto buffer = read_binary(filename, alloc, get);

    auto fbx = std::unique_ptr<ofbx::IScene, void(*)(ofbx::IScene*)>(
        //ofbx::load(buffer.get(), filesize, 0),
        //                                                        todo: do triangulation myself
        ofbx::load(buffer.get(), filesize, static_cast<ofbx::u64>(ofbx::LoadFlags::TRIANGULATE)),
        [](ofbx::IScene* scene) { if(scene) scene->destroy(); }
    );
    buffer.reset();

    return fbx;
}


std::optional<Model> Model::load_from_file(const std::string& filename) {
    //std::vector<Vertex> vertices;
    //std::vector<u16> indices;
    //bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
    //bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;

    std::vector<MeshDataTuple> mdt;
    std::pair<AABB, bool> aabb_hasvalue;

    const auto fbx_scene = load_fbx_model(filename);
    if(!fbx_scene) {
        return std::nullopt;
    }

    for(auto mesh_idx = 0; mesh_idx < fbx_scene->getMeshCount(); ++mesh_idx) {
        const auto* mesh = fbx_scene->getMesh(mesh_idx);
        const auto* geometry = mesh->getGeometry();

        std::vector<Vertex> vertices;
        std::vector<u16> indices;

        // for now, VertexCount == IndexCount because of default triangulation
        for(auto face_idx = 0; face_idx < geometry->getIndexCount(); ++face_idx) {
            const auto vert_idx = [=]() {
                const auto idx = geometry->getFaceIndices()[face_idx];
                return idx < 0 ? -idx - 1 : idx;
            }();

            auto position = glm::vec3{
                geometry->getVertices()[vert_idx].x,
                geometry->getVertices()[vert_idx].y,
                geometry->getVertices()[vert_idx].z,
            };

            if(aabb_hasvalue.second) {
                aabb_hasvalue.first.min = glm::min(position, aabb_hasvalue.first.min);
                aabb_hasvalue.first.max = glm::max(position, aabb_hasvalue.first.max);
            }
            else {
                aabb_hasvalue.first.min = position;
                aabb_hasvalue.first.max = position;
                aabb_hasvalue.second = true;
            }

            auto vertex = Vertex{
                position,
                {
                    geometry->getNormals()[face_idx].x,
                    geometry->getNormals()[face_idx].y,
                    geometry->getNormals()[face_idx].z,
                },
                // todo: handle models that don't specify uvs
                geometry->getUVs(0)
                    ? glm::vec2{geometry->getUVs(0)[face_idx].x, geometry->getUVs(0)[face_idx].y}
                    : glm::vec2{0, 0}
            };

            vertices.emplace_back(vertex);
            indices.push_back(indices.size());
        }

        // todo: use ref
        const auto layout = Vertex::get_layout();
        auto vbh = bgfx::createVertexBuffer(bgfx::copy(vertices.data(), sizeof(Vertex) * vertices.size()), layout);
        auto ibh = bgfx::createIndexBuffer(bgfx::copy(indices.data(), sizeof(u16) * indices.size()));

        // todo: other material params
        glm::vec4 diffuse;
        const auto mesh_mat_cnt = mesh->getMaterialCount();
        if(mesh_mat_cnt == 0)
            diffuse = {.9, .6, .8, 1};
        else if(mesh_mat_cnt == 1) {
            const auto color = mesh->getMaterial(0)->getDiffuseColor();
            diffuse = {color.r, color.g, color.b, 1};
        }
        else {
            assert(geometry->getMaterials());
            // todo: handle multiple materials
            //const auto face_num = geometry->getIndexCount();
            //const auto len_mat_array = face_num / 3;
            //const auto mat_id = geometry->getMaterials()[vert_idx / 3];
            const auto mat_id = 0;
            const auto color = mesh->getMaterial(mat_id)->getDiffuseColor();
            diffuse = {color.r, color.g, color.b, 1};
        }

        auto matrix = mesh->getGlobalTransform();
        glm::mat4 trans;

        //for(auto row = 0; row < 4; ++row) 
        //    for(auto col = 0; col < 4; ++col)
        //        trans[col][row] = matrix.m[row+col*4];

        for(auto i = 0; i < 16; ++i)
            trans[i / 4][i % 4] = matrix.m[i];

        // todo: consider pre-multiplying model matrix
        // init scale = 100, don't know why
        trans *= .01;
        trans[3][3] = 1;

        mdt.emplace_back(MeshDataTuple{vbh, ibh, diffuse, trans});
    }

    Model result;
    //result.vertices = std::move(vertices);
    //result.indices = std::move(indices);
    //result.vbh = vbh;
    //result.ibh = ibh;
    result.mdt = std::move(mdt);
    result.aabb = std::move(aabb_hasvalue.first);

    return result;
}

std::shared_ptr<Model> Model::load_from_file_shared(const std::string& filename) {
    std::optional<Model> result = load_from_file(filename);
    if(result.has_value()) {
        return std::make_shared<Model>(std::move(result.value()));
    }
    else {
        return nullptr;
    }
}
