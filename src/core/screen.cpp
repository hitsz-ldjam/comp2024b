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
    s_screen_impl = new ScreenImpl();

    int x = setup.centered ? (int)SDL_WINDOWPOS_CENTERED : setup.x;
    int y = setup.centered ? (int)SDL_WINDOWPOS_CENTERED : setup.y;

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

glm::ivec2 Screen::size() {
    return s_screen_impl->window.size();
}

void Screen::set_size(i32 width, i32 height) {
    s_screen_impl->window.set_size(width, height);
}

i32 Screen::width() {
    return s_screen_impl->window.width();
}

i32 Screen::height() {
    return s_screen_impl->window.height();
}

glm::ivec2 Screen::draw_size() {
    return s_screen_impl->window.draw_size();
}

i32 Screen::draw_width() {
    return s_screen_impl->window.draw_width();
}

i32 Screen::draw_height() {
    return s_screen_impl->window.draw_height();
}

class Window& Screen::get_window() {
    return s_screen_impl->window;
}

void Screen::show_cursor(bool v) {
    SDL_ShowCursor(v);
}

bool Screen::cursor_shown() {
    return SDL_ShowCursor(-1);
}

void Screen::set_relative_cursor(bool v) {
    SDL_SetRelativeMouseMode(v ? SDL_TRUE : SDL_FALSE);
}

bool Screen::relative_cursor() {
    return SDL_GetRelativeMouseMode();
}
