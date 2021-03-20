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
        return AppState::Destroy;                 \
    }

#define INIT_STATIC_MODULE(name) INIT_STATIC_MODULE_EX(name, )

static bool init_sdl2(const AppSetup& setup) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("failed to initialize SDL2: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

static void quit_sdl2() {
    SDL_Quit();
}

AppBase::~AppBase() noexcept {}

int Launcher::run(AppBase& app) {
    while (curr_state != AppState::Invalid) {
        switch (curr_state) {
        case AppState::Init:
            next_state = app.init();
            break;
        case AppState::Running:
            next_state = app.running();
            break;
        case AppState::Cleanup:
            next_state = app.cleanup();
            break;
        case AppState::Destroy:
            app.destroy(); // we don't care the return value as we always exit after destruction
            next_state = AppState::Invalid;
            break;
        default:
            assert(false && "wrong app state");
        }

        // map default state
        if (next_state == AppState::Default) {
            switch (curr_state) {
            case AppState::Init: next_state = AppState::Running; break;
            case AppState::Running: next_state = AppState::Running; break;
            case AppState::Cleanup: next_state = AppState::Destroy; break;
            case AppState::Destroy: next_state = AppState::Invalid; break;
            default:
                assert(false && "wrong app state");
            }
        }
        curr_state = next_state;
    }

    return 0;
}

AppState App::init() {
    AppSetup setup = on_setup();

    if (!init_sdl2(setup))
        return AppState::Destroy;

    INIT_STATIC_MODULE_EX(Screen, setup)
    INIT_STATIC_MODULE_EX(Gfx, Screen::get_window())
    INIT_STATIC_MODULE(Gui)
    INIT_STATIC_MODULE(Time)
    INIT_STATIC_MODULE(Input)

    on_awake();
    on_start();

    return AppState::Running;
}

AppState App::running() {
    if(quit) {
        return AppState::Cleanup;
    }

    SDL_Event event;
    Input::new_frame();
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return AppState::Cleanup;
        }

        if (!Gui::process_event(event)) {
            Input::process_event(event);
        }
    }

    Time::tick();

    // update gui
    Gui::begin_frame_screen();
    on_gui();
    Gui::end_frame();

    // update scene
    on_update();

    // render scene
    on_render();

    Gui::render();
    Gfx::render();

    return AppState::Running;
}

AppState App::cleanup() {
    return on_closing() ? AppState::Destroy : AppState::Running;
}

AppState App::destroy() {
    on_quit();

    Input::quit();
    Time::quit();
    Gui::quit();
    Gfx::quit();
    Screen::quit();
    quit_sdl2();
    return AppState::Default;
}

void App::request_quit() {
    quit = true;
}
