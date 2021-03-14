#include "app.h"
#include <cstdio>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <bgfx/bgfx.h>
#include "gui.h"

int App::run(int argc, char** argv) {
    (void)argc;
    (void)argv;

    AppSetup setup = on_init();
    if (!init_sdl2(setup))
        return -1;

    if (!init_bgfx(setup))
        return -1;

    if (!Gui::init())
        return -1;

    on_start();
    main_loop();
    on_quit();

    Gui::quit();
    quit_bgfx();
    quit_sdl2();
    return 0;
}

void App::main_loop() {
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
                break;
            }
            Gui::process_event(event);
        }
        Gui::begin_frame(width, height, float(draw_width) / width, float(draw_height) / height);
        on_gui();
        Gui::end_frame();

        if (!Gui::want_capture_mouse()) {
            // todo update mouse
        }

        if(!Gui::want_capture_keyboard()) {
            // todo update keyboard
        }

        on_update();

        render();

        if (quit) {
            quit = on_closing();
        }
    }
}

void App::render() {
    on_render();
    Gui::render();
    bgfx::touch(0);
    bgfx::frame();
}

bool App::init_sdl2(const AppSetup& setup) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("failed to initialize SDL2: %s\n", SDL_GetError());
        return false;
    }

    int x = setup.centered ? (int)SDL_WINDOWPOS_CENTERED : setup.x;
    int y = setup.centered ? (int)SDL_WINDOWPOS_CENTERED : setup.y;

    window = SDL_CreateWindow(setup.title.c_str(),
                              x,
                              y,
                              setup.width,
                              setup.height,
                              setup.flags);

    if (window == nullptr) {
        printf("failed to create window:%s\n", SDL_GetError());
        return false;
    }

    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    width  = w;
    height = h;
    SDL_GL_GetDrawableSize(window, &w, &h);
    draw_width  = w;
    draw_height = h;

    return true;
}

void App::quit_sdl2() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool App::init_bgfx(const AppSetup& setup) {
    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version)
    if (!SDL_GetWindowWMInfo(window, &wminfo)) {
        printf("failed to get window wminfo: %s\n", SDL_GetError());
        return -1;
    }

    // init bgfx
    bgfx::PlatformData pd;
    memset(&pd, 0, sizeof(pd));
    pd.ndt = nullptr;
    pd.nwh = wminfo.info.win.window;

    bgfx::Init init;
    init.type              = bgfx::RendererType::Direct3D11;
    init.resolution.width  = setup.width;
    init.resolution.height = setup.height;
    init.resolution.reset  = BGFX_RESET_VSYNC;
    init.platformData      = pd;
    if (!bgfx::init(init))
        return false;

    bgfx::setViewRect(0, 0, 0, setup.width, setup.height);
    bgfx::setViewClear(0,
                       BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                       0x303030ff,
                       1.0f,
                       0);

    return true;
}

void App::quit_bgfx() {
    bgfx::shutdown();
}

void App::request_quit() {
    quit = true;
}
