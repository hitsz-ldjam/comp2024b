#include "gfx.h"
#include <SDL2/SDL_syswm.h>
#include <bgfx/bgfx.h>
#include <cstdio>
#include "window.h"

enum ViewId : u16 {
    VID_Main = 0,
    // maybe more than one scene view
    VID_GUI = 32
};

bool Gfx::init(Window& window) {
    SDL_Window* raw_win = window.get_raw();
    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version)
    if (!SDL_GetWindowWMInfo(raw_win, &wminfo)) {
        printf("failed to get window wminfo: %s\n", SDL_GetError());
        return false;
    }

    i32 width  = window.draw_width();
    i32 height = window.draw_height();

    // init bgfx
    bgfx::PlatformData pd;
    memset(&pd, 0, sizeof(pd));
    pd.ndt = nullptr;
    pd.nwh = wminfo.info.win.window;

    bgfx::Init init;
    init.type              = bgfx::RendererType::Direct3D11;
    init.resolution.width  = width;
    init.resolution.height = height;
    init.resolution.reset  = BGFX_RESET_VSYNC;
    init.platformData      = pd;
    if (!bgfx::init(init))
        return false;

    bgfx::setViewRect(VID_Main, 0, 0, width, height);
    bgfx::setViewClear(VID_Main, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
    return true;
}

void Gfx::quit() {
    bgfx::shutdown();
}

u16 Gfx::main_view() {
    return VID_Main;
}

u16 Gfx::gui_view() {
    return VID_GUI;
}
void Gfx::render() {
    bgfx::touch(VID_Main);
    bgfx::frame();
}
