#include "app.h"
#include <SDL2/SDL.h>
#include <imgui/imgui.h>

class Demo : public App {
public:
    AppSetup on_init() override {
        return AppSetup{
            .title    = "Demo",
            .centered = true,
            .width    = 1280,
            .height   = 720,
            .flags    = SDL_WINDOW_SHOWN
        };
    }

    void on_start() override {
    }

    void on_update() override {
    }

    void on_render() override {
    }

    void on_gui() override {
        ImGui::ShowDemoWindow();
    }

    bool on_closing() override {
        return true;
    }

    void on_quit() override {
    }
};

int main(int argc, char** argv) {
    return Demo().run(argc, argv);
}