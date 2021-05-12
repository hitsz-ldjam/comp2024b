#include "camera_control.h"
#include "components/camera_control_data.h"
#include "components/transform.h"
#include "components/camera.h"

#include "core/input.h"
#include "core/times.h"

#include "entt/entity/registry.hpp"

namespace Systems {
    void camera_control(entt::registry& scene) {
        auto cameras = scene.view<Transform, Camera, CameraControlData>();
        cameras.each([](entt::entity entity, Transform& trans, Camera& camera, CameraControlData& control) {
            glm::vec3 dir = glm::zero<glm::vec3>();
            if (Input::key_repeat(KeyCode::W)) {
                dir += Transform::FORWARD;
            }
            if (Input::key_repeat(KeyCode::S)) {
                dir += Transform::BACK;
            }
            if (Input::key_repeat(KeyCode::A)) {
                dir += Transform::LEFT;
            }
            if (Input::key_repeat(KeyCode::D)) {
                dir += Transform::RIGHT;
            }
            if (Input::key_repeat(KeyCode::E)) {
                dir += Transform::UP;
            }
            if (Input::key_repeat(KeyCode::Q)) {
                dir += Transform::DOWN;
            }

            if (!glm::all(glm::epsilonEqual(dir, glm::zero<glm::vec3>(), 0.0001f))) {
                dir = glm::normalize(dir);
                trans.translate_in_local(dir * Time::delta() * control.walk_speed);
            }

            glm::ivec2 mouse = Input::mouse_pos_delta();
            if (mouse.x != 0 || mouse.y != 0) {
                glm::quat rotation = glm::identity<glm::quat>();
                rotation           = glm::rotate(rotation, glm::radians(mouse.x * Time::delta() * control.rotate_speed), Transform::UP);
                rotation           = glm::rotate(rotation, glm::radians(mouse.y * Time::delta() * control.rotate_speed), trans.right());
                trans.rotate(rotation);
            }
        });
    }
} // namespace Systems