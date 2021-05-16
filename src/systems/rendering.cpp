#include "rendering.h"
#include <entt/entity/registry.hpp>
#include <bgfx/bgfx.h>
#include <glm/gtc/type_ptr.hpp>

#include "core/gfx.h"
#include "core/graphic/model.h"
#include "core/graphic/shader.h"
#include "core/screen.h"

#include "components/transform.h"
#include "components/camera.h"
#include "components/render.h"

namespace Systems {
    void rendering(entt::registry& scene) {
        for (auto&& [entity, trans, camera] : scene.view<const Transform, const Camera>().each()) {
            glm::mat4 view = trans.view_matrix();
            glm::mat4 proj = camera.matrix();
            bgfx::setViewTransform(Gfx::main_view(), glm::value_ptr(view), glm::value_ptr(proj));
            bgfx::setViewRect(Gfx::main_view(), 0, 0, Screen::draw_width(), Screen::draw_height());

            auto objects = scene.view<const Transform, const RenderComponent>();
            objects.each([&](const Transform& trans, const RenderComponent& render) {
                if (render.model == nullptr || render.shader == nullptr) {
                    perror("null Model or Shader in RenderComponent\n");
                    return;
                }

                for (const auto& mdt : render.model->mdt) {
                    bgfx::setTransform(glm::value_ptr(mdt.transform));
                    bgfx::setVertexBuffer(0, mdt.vbh);
                    bgfx::setIndexBuffer(mdt.ibh);
                    // todo: move to material
                    const float gloss = 32;
                    glm::vec4 diffuse = mdt.diffuse;
                    diffuse.a = gloss;
                    bgfx::setUniform(render.uniform, glm::value_ptr(diffuse));
                    bgfx::setState(BGFX_STATE_WRITE_RGB
                                   | BGFX_STATE_WRITE_Z
                                   | BGFX_STATE_DEPTH_TEST_LESS); // don't cull since model is corrupt
                    bgfx::submit(Gfx::main_view(), render.shader->handle());
                }
            });

            // we only support only one camera in scene for now
            break;
        }
    }
} // namespace Systems
