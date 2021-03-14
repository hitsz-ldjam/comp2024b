#pragma once
#include <string>
#include "types.h"

struct AppSetup {
    std::string title = u8"App";
    bool centered     = true;
    i32 x             = 0;
    i32 y             = 0;
    i32 width         = 800;
    i32 height        = 600;
    u32 flags         = 0;
};

class App {
public:
    App()          = default;
    virtual ~App() = default;

    virtual AppSetup on_init() = 0;
    virtual void on_start(){};
    virtual void on_update(){};
    virtual void on_render(){};
    virtual void on_gui(){};
    virtual bool on_closing() { return true; };
    virtual void on_quit(){};

    int run(int argc, char** argv);

    void request_quit();

private:
    bool init_sdl2(const AppSetup& setup);
    void quit_sdl2();
    bool init_bgfx(const AppSetup& setup);
    void quit_bgfx();
    void main_loop();
    void render();

    class SDL_Window* window = nullptr;
    u32 width                = 0;
    u32 height               = 0;
    u32 draw_width           = 0;
    u32 draw_height          = 0;
    bool quit                = false;
};