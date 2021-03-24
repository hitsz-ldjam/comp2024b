#pragma once
#include "types.h"
#include <glm/vec2.hpp>

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
    glm::ivec2 pos() const;
    i32 pos_x() const;
    i32 pos_y() const;
    glm::ivec2 size() const;
    i32 width() const;
    i32 height() const;
    glm::ivec2 draw_size() const;
    i32 draw_width() const;
    i32 draw_height() const;
    void set_pos(i32 x, i32 y);
    void set_size(i32 w, i32 h);
    bool is_valid() const { return window != nullptr; }
    SDL_Window* get_raw() const { return window; }
    u32 get_id() const { return id; }

private:
    SDL_Window* window = nullptr;
    u32 id             = 0;
};