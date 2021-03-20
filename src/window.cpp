#include "window.h"
#include <SDL2/SDL_video.h>
#include <cassert>

Window::~Window() {
    if (window) {
        SDL_DestroyWindow(window);
    }
}

Window::Window(const char* title, i32 x, i32 y, i32 w, i32 h, u32 flags) {
    window = SDL_CreateWindow(title, x, y, w, h, flags);
    id     = SDL_GetWindowID(window);
}

Window::Window(Window&& other) noexcept
    : window(other.window),
      id(other.id) {
    other.window = nullptr;
    other.id     = 0;
}

Window& Window::operator=(Window&& other) noexcept {
    window       = other.window;
    id           = other.id;
    other.window = nullptr;
    other.id     = 0;
    return *this;
}

glm::ivec2 Window::pos() const {
    glm::ivec2 result;
    SDL_GetWindowPosition(window, &result.x, &result.y);
    return result;
}

glm::ivec2 Window::size() const {
    glm::ivec2 result;
    SDL_GetWindowSize(window, &result.x, &result.y);
    return result;
}

glm::ivec2 Window::draw_size() const {
    glm::ivec2 result;
    SDL_GL_GetDrawableSize(window, &result.x, &result.y);
    return result;
}

i32 Window::pos_x() const {
    return pos().x;
}

i32 Window::pos_y() const {
    return pos().y;
}

i32 Window::width() const {
    return size().x;
}

i32 Window::height() const {
    return size().y;
}

i32 Window::draw_width() const {
    return draw_size().x;
}

i32 Window::draw_height() const {
    return draw_size().y;
}

void Window::set_pos(i32 x, i32 y) {
    assert(window != nullptr);
    SDL_SetWindowPosition(window, x, y);
}

void Window::set_size(i32 w, i32 h) {
    assert(window != nullptr);
    SDL_SetWindowSize(window, w, h);
}
