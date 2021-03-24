#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform {
public:
    Transform() = default;

    Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
        : position(position),
          rotation(rotation),
          scale(scale) {}

    glm::mat4 matrix() const;
    glm::mat4 view_matrix() const;
    glm::vec3 forward() const;
    glm::vec3 right() const;
    glm::vec3 up() const;
    Transform& translate(const glm::vec3& trans);
    Transform& rotate(const glm::quat& r);
    Transform& translate_in_local(const glm::vec3& trans);
    Transform& rotate_in_local(const glm::quat& r);
    Transform& face_towards(const glm::vec3& target, const glm::vec3& up = UP);
    static Transform look_at(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up = UP);

    static const glm::vec3 FORWARD;
    static const glm::vec3 BACK;
    static const glm::vec3 UP;
    static const glm::vec3 DOWN;
    static const glm::vec3 RIGHT;
    static const glm::vec3 LEFT;

    glm::vec3 position = glm::vec3(0);
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    glm::vec3 scale    = glm::vec3(1);
};
