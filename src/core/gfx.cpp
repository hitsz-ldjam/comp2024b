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

struct GfxImpl {
    // back-buffer settings
    i32 width;
    i32 height;
    u32 flags;
    bgfx::TextureFormat::Enum format;
};

static GfxImpl* s_gfx_impl = nullptr;

bool Gfx::init(Window& window) {
    assert(s_gfx_impl == nullptr && "gfx is initialized twice");
    s_gfx_impl = new GfxImpl();

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

    s_gfx_impl->width  = width;
    s_gfx_impl->height = height;
    s_gfx_impl->flags  = init.resolution.reset;
    s_gfx_impl->format = init.resolution.format;

    bgfx::setViewRect(VID_Main, 0, 0, width, height);
    bgfx::setViewClear(VID_Main, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
    return true;
}

void Gfx::quit() {
    bgfx::shutdown();
    delete s_gfx_impl;
    s_gfx_impl = nullptr;
}

u16 Gfx::main_view() {
    return VID_Main;
}

u16 Gfx::gui_view() {
    return VID_GUI;
}

void Gfx::before_render(i32 width, i32 height) {
    // check if we need to resize bgfx back-buffer
    if (width != s_gfx_impl->width || height != s_gfx_impl->height) {
        s_gfx_impl->width  = width;
        s_gfx_impl->height = height;
        bgfx::reset((u32)s_gfx_impl->width, (u32)s_gfx_impl->height, s_gfx_impl->flags, s_gfx_impl->format);
        // set view rect to cover the whole back-buffer, user can change this later
        bgfx::setViewRect(VID_Main, 0, 0, (u16)width, (u16)height);
    }
}

void Gfx::render() {
    bgfx::touch(VID_Main);
    bgfx::frame();
}
