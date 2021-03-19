#pragma once
#include <string>
#include "setup.h"

class App {
public:
    App()          = default;
    virtual ~App() = default;
    App(const App&) = delete;

    virtual AppSetup on_init() = 0;
    virtual void on_start() {}
    virtual void on_update() {}
    virtual void on_render() {}
    virtual void on_gui() {}
    virtual bool on_closing() { return true; }
    virtual void on_quit() {}

    int run(int argc, char** argv);

    void request_quit();

private:
    bool init_sdl2(const AppSetup& setup);
    void quit_sdl2();
    void main_loop();
    void render();

    bool quit = false;
};