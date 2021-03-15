#include "app.h"
#include <SDL2/SDL.h>
#include <imgui/imgui.h>

#include "times.h"

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
        ImGui::SetNextWindowSize(ImVec2(200, 100));
        ImGui::Begin("time test", nullptr, ImGuiWindowFlags_NoCollapse);
        {
            ImGui::Text("delta time: %f", Time::delta());
            ImGui::Text("real time: %f", Time::real());
        }
        ImGui::End();
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