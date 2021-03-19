#include "app.h"
#include <cstdio>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <bgfx/bgfx.h>
#include "screen.h"
#include "gfx.h"
#include "gui.h"
#include "times.h"
#include "input.h"

#define INIT_STATIC_MODULE_EX(name, ...)       \
    if (!(name::init(__VA_ARGS__))) {          \
        fprintf(stderr, #name " init failed"); \
        return -1;                             \
    }

#define INIT_STATIC_MODULE(name) INIT_STATIC_MODULE_EX(name,)

int App::run(int argc, char** argv) {
    (void)argc;
    (void)argv;

    AppSetup setup = on_init();

    if (!init_sdl2(setup))
        return -1;

    INIT_STATIC_MODULE_EX(Screen, setup)
    INIT_STATIC_MODULE_EX(Gfx, Screen::get_window())
    INIT_STATIC_MODULE(Gui)
    INIT_STATIC_MODULE(Time)
    INIT_STATIC_MODULE(Input)

    on_start();
    main_loop();
    on_quit();

    Input::quit();
    Time::quit();
    Gui::quit();
    Gfx::quit();
    Screen::quit();
    quit_sdl2();
    return 0;
}

void App::main_loop() {
    SDL_Event event;
    while (!quit) {
        Input::new_frame();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
                break;
            }

            if (!Gui::process_event(event)) {
                Input::process_event(event);
            }
        }

        Time::tick();

        Gui::begin_frame_screen();
        on_gui();
        Gui::end_frame();

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
    Gfx::render();
}

bool App::init_sdl2(const AppSetup& setup) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("failed to initialize SDL2: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void App::quit_sdl2() {
    SDL_Quit();
}

void App::request_quit() {
    quit = true;
}
