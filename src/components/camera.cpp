#include "camera.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

float Camera::fov() const {
    return atan(1.0f / mat[0][0]) * 2.0f;
}

Camera& Camera::set_fov(float fov) {
    float old_m11 = mat[1][1];
    mat[1][1]     = 1.0f / tan(fov / 2.0f);
    mat[0][0]     = mat[0][0] / old_m11 * mat[1][1];
    return *this;
}

float Camera::aspect() const {
    return mat[1][1] / mat[0][0];
}

Camera& Camera::set_aspect(float aspect) {
    mat[0][0] = mat[1][1] / aspect;
    return *this;
}

float Camera::znear() const {
    return -mat[3][2] / mat[2][2];
}

Camera& Camera::set_znear(float znear) {
    set_znear_zfar(znear, zfar());
    return *this;
}

float Camera::zfar() const {
    return mat[3][2] / (1.0f - mat[2][2]);
}

Camera& Camera::set_zfar(float zfar) {
    set_znear_zfar(znear(), zfar);
    return *this;
}

Camera& Camera::set_znear_zfar(float znear, float zfar) {
    mat[2][2] = zfar / (zfar - znear);
    mat[3][2] = -zfar * znear / (zfar - znear);
    return *this;
}

Camera Camera::perspective(float fov, float aspect, float znear, float zfar) {
    return Camera(glm::perspectiveLH_ZO(fov, aspect, znear, zfar));
}

Camera Camera::orthographic(float left, float right, float bottom, float top, float znear, float zfar) {
    return Camera(glm::orthoLH_ZO(left, right, bottom, top, znear, zfar));
}

Camera Camera::frustum(float left, float right, float bottom, float top, float znear, float zfar) {
    return Camera(glm::frustumLH_ZO(left, right, bottom, top, znear, zfar));
}
Camera Camera::standard_2d(float w, float h, float znear, float zfar) {
    return Camera::orthographic(-w / 2, w / 2, -h / 2, h / 2, znear, zfar);
}

Camera Camera::standard_3d(float w, float h, float znear, float zfar) {
    return Camera::frustum(-w / 2, w / 2, -h / 2, h / 2, znear, zfar);
}
