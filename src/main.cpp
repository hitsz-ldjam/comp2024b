#include "core/app.h"
#include <SDL2/SDL.h>
#include <imgui/imgui.h>
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <entt/entt.hpp>

#include "core/times.h"
#include "core/input.h"
#include "core/gfx.h"
#include "core/screen.h"
#include "core/window.h"
#include "core/gui.h"

#include "core/graphic/model.h"
#include "core/graphic/shader.h"

#include "components/transform.h"
#include "components/camera.h"
#include "components/camera_control_data.h"
#include "components/render.h"

#include "systems/camera_control.h"
#include "systems/rendering.h"

#include <fstream>
#include <memory>

class Demo : public App {
public:
    AppSetup on_setup() override {
        AppSetup as;
        as.title    = "Demo";
        as.centered = true;
        as.width    = 1280;
        as.height   = 720;
        as.flags    = SDL_WINDOW_SHOWN;
        return as;
    }

    void on_awake() override {
        model           = Model::load_from_file_shared("./res/models/A_full.fbx");
        u_diffuse_color = bgfx::createUniform("u_diffuse_color", bgfx::UniformType::Vec4);
        program         = std::make_shared<Shader>("./res/shaders/vs_unlit.bin", "./res/shaders/fs_unlit.bin");
    }

    void on_start() override {
        // add entities to scene
        auto teapot = scene.create();
        scene.emplace<Transform>(teapot);
        auto& render_comp = scene.emplace<RenderComponent>(teapot, RenderComponent(model, program));
        render_comp.uniform = u_diffuse_color;

        camera = scene.create();
        scene.emplace<Camera>(camera, Camera::perspective(glm::radians(60.f), (float)Screen::width() / Screen::height(), .1f, 300.f));
        scene.emplace<Transform>(camera, Transform::look_at(glm::vec3(-15, 15, -20), glm::vec3(0, 0, 0)));
        scene.emplace<CameraControlData>(camera, CameraControlData{ 20.0f, 30.0f });
    }

    void on_update() override {
        if (!process_scene_input())
            return;
        Systems::camera_control(scene);
    }

    void on_render() override {
        Systems::rendering(scene);
    }

    void on_gui() override {
        ImGui::Begin("Test", nullptr, ImGuiWindowFlags_NoCollapse);
        {
            if (ImGui::BeginTabBar("menu")) {
                if (ImGui::BeginTabItem("Help")) {
                    gui_help_tab();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Debug")) {
                    gui_debug_tab();
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    bool on_closing() override {
        return true;
    }

    void on_quit() override {
        scene.clear();

        program.reset();
        bgfx::destroy(u_diffuse_color);
        model.reset();
    }

private:
    void gui_help_tab() {
        ImGui::Text("Press left ALT to switch between scene and ui");
        ImGui::NewLine();
        ImGui::Text("Use WASD to move forward/left/back/right");
        ImGui::Text("Use E/Q to move up and down");
        ImGui::Text("Use mouse to look around");
        ImGui::NewLine();
        ImGui::Text("Press ESC to quit");
    }

    void gui_debug_tab() {
        if (ImGui::Button("Quit")) {
            request_quit();
        }
        if (ImGui::CollapsingHeader("Screen")) {
            ImGui::Text("window size: (%d, %d)", Screen::width(), Screen::height());
            ImGui::Text("window drawable size: (%d, %d)", Screen::draw_width(), Screen::draw_height());
            ImGui::Text("change window size");
            static int resolve = 1;
            static int last    = resolve;
            ImGui::RadioButton("800x600", &resolve, 0);
            ImGui::RadioButton("1280x720", &resolve, 1);
            ImGui::RadioButton("1440x900", &resolve, 2);
            if (last != resolve) {
                Camera& c = scene.get<Camera>(camera);
                switch (resolve) {
                case 0:
                    Screen::set_size(800, 600);
                    c.set_aspect(800.0f / 600.0f);
                    break;
                case 1:
                    Screen::set_size(1280, 720);
                    c.set_aspect(1280.0f / 720.0f);
                    break;
                case 2:
                    Screen::set_size(1440, 960);
                    c.set_aspect(1440.0f / 960.0f);
                    break;
                }
                last = resolve;
            }
        }

        if (ImGui::CollapsingHeader("Gfx")) {
            ImGui::Text("main view: %u", Gfx::main_view());
            ImGui::Text("gui view: %u", Gfx::gui_view());
        }

        if (ImGui::CollapsingHeader("Time")) {
            ImGui::Text("delta time: %f", Time::delta());
            ImGui::Text("real time: %f", Time::real());
        }

        if (ImGui::CollapsingHeader("Input")) {
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

    bool process_scene_input() {
        // use left ALT to switch between scene and gui
        if (Input::key_pressed(KeyCode::LAlt)) {
            gui_capture_input = !gui_capture_input;
            Gui::set_capture_keyboard(gui_capture_input);
            Gui::set_capture_mouse(gui_capture_input);
            Screen::set_relative_cursor(!gui_capture_input);
        }

        if (Input::key_pressed(KeyCode::Escape)) {
            request_quit();
            return false;
        }

        if (gui_capture_input)
            return false;

        return true;
    }

    bool gui_capture_input = true;
    std::shared_ptr<Model> model;
    std::shared_ptr<Shader> program;
    bgfx::UniformHandle u_diffuse_color = BGFX_INVALID_HANDLE;

    // todo put these into base class
    entt::registry scene;
    entt::entity camera;
};

LAUNCH_APP(Demo)
