#include "screen.h"
#include <cassert>
#include <string>
#include <SDL2/SDL.h>
#include "window.h"

struct ScreenImpl {
    Window window;
};

static ScreenImpl* s_screen_impl = nullptr;

bool Screen::init(const AppSetup& setup) {
    assert(s_screen_impl == nullptr && "screen is initialized twice");
    s_screen_impl         = new ScreenImpl();

    int x                 = setup.centered ? (int)SDL_WINDOWPOS_CENTERED : setup.x;
    int y                 = setup.centered ? (int)SDL_WINDOWPOS_CENTERED : setup.y;

    s_screen_impl->window = Window(setup.title.c_str(),
                                   x,
                                   y,
                                   setup.width,
                                   setup.height,
                                   setup.flags);
    return true;
}

void Screen::quit() {
    delete s_screen_impl;
    s_screen_impl = nullptr;
}

i32 Screen::width() {
    return s_screen_impl->window.width();
}

i32 Screen::height() {
    return s_screen_impl->window.height();
}

class Window& Screen::get_window() {
    return s_screen_impl->window;
}

i32 Screen::draw_width() {
    return s_screen_impl->window.draw_width();
}

i32 Screen::draw_height() {
    return s_screen_impl->window.draw_height();
}
