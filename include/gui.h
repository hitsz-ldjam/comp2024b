#pragma once
#include "types.h"

class Gui {
public:
    static bool init();
    static void quit();
    static void begin_frame(i32 width, i32 height, float w_scale, float h_scale);
    static void end_frame();
    static void process_event(const union SDL_Event& event);
    static void render();
    static bool want_capture_mouse();
    static bool want_capture_keyboard();
};