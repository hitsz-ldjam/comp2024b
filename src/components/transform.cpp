#include "transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

const glm::vec3 Transform::FORWARD = glm::vec3(0, 0, 1);
const glm::vec3 Transform::BACK = glm::vec3(0, 0, -1);
const glm::vec3 Transform::UP = glm::vec3(0, 1, 0);
const glm::vec3 Transform::DOWN = glm::vec3(0, -1, 0);
const glm::vec3 Transform::RIGHT = glm::vec3(1, 0, 0);
const glm::vec3 Transform::LEFT = glm::vec3(-1, 0, 0);

glm::mat4 Transform::matrix() const {
    glm::mat4 mat = glm::mat4(1);
    mat           = glm::translate(mat, position);
    mat           = mat * glm::toMat4(rotation);
    mat           = glm::scale(mat, scale);
    return mat;
}

glm::mat4 Transform::view_matrix() const {
    glm::mat4 mat = glm::transpose(glm::toMat4(rotation));
    glm::vec3 r   = glm::vec3(mat[0][0], mat[1][0], mat[2][0]);
    glm::vec3 u   = glm::vec3(mat[0][1], mat[1][1], mat[2][1]);
    glm::vec3 f   = glm::vec3(mat[0][2], mat[1][2], mat[2][2]);
    mat[3][0] = -glm::dot(r, position);
    mat[3][1] = -glm::dot(u, position);
    mat[3][2] = -glm::dot(f, position);
    return mat;
}

Transform& Transform::translate(const glm::vec3& trans) {
    position += trans;
    return *this;
}

Transform& Transform::rotate(const glm::quat& r) {
    rotation = r * rotation;
    return *this;
}

Transform& Transform::translate_in_local(const glm::vec3& trans) {
    position += rotation * trans;
    return *this;
}

Transform& Transform::rotate_in_local(const glm::quat& r) {
    rotation *= r;
    return *this;
}
glm::vec3 Transform::forward() const {
    return rotation * FORWARD;
}
glm::vec3 Transform::right() const {
    return rotation * RIGHT;
}
glm::vec3 Transform::up() const {
    return rotation * UP;
}

Transform& Transform::face_towards(const glm::vec3& target, const glm::vec3& up) {
    glm::vec3 dir = glm::normalize(target - position);
    rotation = glm::quatLookAtLH(dir, up);
    return *this;
}

Transform Transform::look_at(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up) {
    glm::vec3 dir = glm::normalize(target - eye);
    glm::quat rotation = glm::quatLookAtLH(dir, up);
    return Transform(eye, rotation, glm::vec3(1));
}
