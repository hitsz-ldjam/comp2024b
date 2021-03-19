#pragma once
#include "types.h"

struct SDL_Window;

class Window {
public:
    ~Window();
    Window() = default;
    Window(const char* title, i32 x, i32 y, i32 w, i32 h, u32 flags);
    Window(const Window& other) = delete;
    Window& operator=(const Window& other) = delete;
    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;
    i32 pos_x() const { return x_; }
    i32 pos_y() const { return y_; }
    i32 width() const { return w_; }
    i32 height() const { return h_; }
    i32 draw_width() const { return draw_w_; }
    i32 draw_height() const { return draw_h_; }
    void set_pos(i32 x, i32 y);
    void set_size(i32 w, i32 h);
    bool is_valid() const { return window != nullptr; }
    SDL_Window* get_raw() const { return window; }

private:
    SDL_Window* window = nullptr;
    i32 x_             = 0;
    i32 y_             = 0;
    i32 w_             = 0;
    i32 h_             = 0;
    i32 draw_w_        = 0;
    i32 draw_h_        = 0;
};