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
    SDL_GetWindowSize(window, &w_, &h_);
    SDL_GL_GetDrawableSize(window, &draw_w_, &draw_h_);
}

Window::Window(Window&& other) noexcept
    : window(other.window),
      x_(other.x_),
      y_(other.y_),
      w_(other.w_),
      h_(other.h_),
      draw_w_(other.draw_w_),
      draw_h_(other.draw_h_) {
    other.window = nullptr;
}

Window& Window::operator=(Window&& other) noexcept {
    window       = other.window;
    x_           = other.x_;
    y_           = other.y_;
    w_           = other.w_;
    h_           = other.h_;
    draw_w_      = other.draw_w_;
    draw_h_      = other.draw_h_;
    other.window = nullptr;
    return *this;
}

void Window::set_pos(i32 x, i32 y) {
    assert(window != nullptr);
    SDL_SetWindowPosition(window, x, y);
    SDL_GetWindowPosition(window, &x_, &y_);
}

void Window::set_size(i32 w, i32 h) {
    assert(window != nullptr);
    SDL_SetWindowSize(window, w, h);
    SDL_GetWindowSize(window, &w_, &h_);
    SDL_GL_GetDrawableSize(window, &draw_w_, &draw_h_);
}
