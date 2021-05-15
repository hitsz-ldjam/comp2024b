#pragma once
#include "types.h"

class Window;

class Gfx {
public:
    static bool init(Window& window);
    static void quit();
    static void before_render(i32 width, i32 height);
    static void render();
    static u16 main_view();
    static u16 pe_view();
    static u16 gui_view();
};