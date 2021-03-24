#pragma once
#include <glm/mat4x4.hpp>

class Camera {
public:
    Camera() = default;
    Camera(const glm::mat4& mat)
        : mat(mat) {}

    const glm::mat4& matrix() const { return mat; }
    glm::mat4& matrix() { return mat; }
    float fov() const;
    Camera& set_fov(float fov);
    float aspect() const;
    Camera& set_aspect(float aspect);
    float znear() const;
    Camera& set_znear(float znear);
    float zfar() const;
    Camera& set_zfar(float zfar);
    Camera& set_znear_zfar(float znear, float zfar);

    static Camera standard_2d(float w, float h, float znear, float zfar);
    static Camera standard_3d(float w, float h, float znear, float zfar);
    static Camera perspective(float fov, float aspect, float znear, float zfar);
    static Camera frustum(float left, float right, float bottom, float top, float znear, float zfar);
    static Camera orthographic(float left, float right, float bottom, float top, float znear, float zfar);

private:
    glm::mat4 mat = glm::mat4(1);
};
