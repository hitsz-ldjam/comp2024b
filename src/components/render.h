#pragma once

#include <bgfx/bgfx.h>
#include <memory>

struct RenderComponent {
    std::shared_ptr<class Model> model;
    std::shared_ptr<class Shader> shader;

    bgfx::UniformHandle uniform; // move this into Material

    RenderComponent() = default;

    RenderComponent(std::shared_ptr<Model> model, std::shared_ptr<Shader> shader)
        : model(std::move(model)),
          shader(std::move(shader)) {}
};