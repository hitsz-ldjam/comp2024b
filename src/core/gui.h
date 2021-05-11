#pragma once
#include "types.h"

class Gui {
public:
    static bool init();
    static void quit();
    static void begin_frame(i32 width, i32 height, float w_scale, float h_scale);
    static void end_frame();
    static bool process_event(const union SDL_Event& event);
    static bool want_capture_mouse();
    static bool want_capture_keyboard();
    static void set_capture_mouse(bool v);
    static void set_capture_keyboard(bool v);
    static void render();
};