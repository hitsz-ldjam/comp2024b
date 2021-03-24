#pragma once
#include <string>
#include "setup.h"

enum class AppState {
    Default,
    Init,
    Running,
    Cleanup,
    Destroy,
    Invalid
};

class AppBase {
public:
    AppBase()               = default;
    virtual ~AppBase()      = 0;
    AppBase(const AppBase&) = delete;

    virtual AppState init() { return AppState::Default; }
    virtual AppState running() { return AppState::Default; }
    virtual AppState cleanup() { return AppState::Default; }
    virtual AppState destroy() { return AppState::Default; }
};

class Launcher {
public:
    Launcher() = default;

    int run(AppBase& app);

private:
    AppState curr_state = AppState::Init;
    AppState next_state = AppState::Default;
};

// A simple app base class that initializes all modules and provides basic functionality and a main loop
class App : public AppBase {
public:
    AppState init() override;
    AppState running() override;
    AppState cleanup() override;
    AppState destroy() override;

    virtual AppSetup on_setup() = 0;
    virtual void on_awake() {}
    virtual void on_start() {}
    virtual void on_update() {}
    virtual void on_render() {}
    virtual void on_gui() {}
    virtual bool on_closing() { return true; }
    virtual void on_quit() {}

    void request_quit();

    bool quit = false;
};

#define LAUNCH_APP(name)                         \
    int main(int argc, char** argv) {            \
        setbuf(stdout, 0); /* for debug print */ \
        name app;                                \
        return Launcher().run(app);              \
    }