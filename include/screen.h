#pragma once
#include "types.h"
#include "setup.h"

class Screen {
public:
    static bool init(const AppSetup& setup);
    static void quit();
    static i32 width();
    static i32 height();
    static i32 draw_width();
    static i32 draw_height();
    static class Window& get_window();
};