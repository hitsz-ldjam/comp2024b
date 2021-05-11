#pragma once
#include "types.h"
#include "setup.h"
#include <glm/vec2.hpp>

class Screen {
public:
    static bool init(const AppSetup& setup);
    static void quit();
    static glm::ivec2 size();
    static void set_size(i32 width, i32 height);
    static i32 width();
    static i32 height();
    static glm::ivec2 draw_size();
    static i32 draw_width();
    static i32 draw_height();
    static void show_cursor(bool v);
    static bool cursor_shown();
    static void set_relative_cursor(bool v);
    static bool relative_cursor();
    static class Window& get_window();
};