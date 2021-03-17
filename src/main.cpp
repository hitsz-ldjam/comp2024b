#include "app.h"
#include <SDL2/SDL.h>
#include <imgui/imgui.h>
#include <bgfx/bgfx.h>

#include "times.h"
#include "input.h"

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
        ImGui::Begin("Test", nullptr, ImGuiWindowFlags_NoCollapse);
        {
            if (ImGui::CollapsingHeader("Time", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanFullWidth)) {
                ImGui::Text("delta time: %f", Time::delta());
                ImGui::Text("real time: %f", Time::real());
            }

            if (ImGui::CollapsingHeader("Input", ImGuiTreeNodeFlags_SpanFullWidth)) {
                ImGui::Text("Mouse");
                glm::ivec2 xy;
                xy = Input::mouse_pos();
                ImGui::Text("mouse pos: (%d, %d)", xy.x, xy.y);
                xy = Input::mouse_pos_delta();
                ImGui::Text("mouse delta pos: (%d, %d)", xy.x, xy.y);
                xy           = Input::mouse_wheel();
                static i32 x = 0, y = 0;
                x += xy.x;
                y += xy.y;
                ImGui::Text("mouse wheel: (%d, %d)", x, y);
                ImGui::Text("mouse button: %d | %d | %d | %d | %d",
                            Input::mouse_button_repeat(0),
                            Input::mouse_button_repeat(1),
                            Input::mouse_button_repeat(2),
                            Input::mouse_button_repeat(3),
                            Input::mouse_button_repeat(4));
                ImGui::Text("any mouse button pressed: %d", Input::any_mouse_button_pressed());
                ImGui::Text("any mouse button repeat: %d", Input::any_mouse_button_repeat());
                ImGui::Text("any mouse button released: %d", Input::any_mouse_button_released());

                ImGui::Separator();
                ImGui::Text("Keyboard");
                ImGui::Text("any key pressed: %d", Input::any_key_pressed());
                ImGui::Text("any key repeat: %d", Input::any_key_repeat());
                ImGui::Text("any key released: %d", Input::any_key_released());
            }
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
    setbuf(stdout, 0); // for debug print
    return Demo().run(argc, argv);
}