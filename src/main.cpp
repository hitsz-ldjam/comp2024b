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

#include <fstream>
#include <memory>

class Demo : public App {
public:
    AppSetup on_setup() override {
        // msvc does not support designated initializer
        AppSetup as;
        as.title    = "Demo";
        as.centered = true;
        as.width    = 1280;
        as.height   = 720;
        as.flags    = SDL_WINDOW_SHOWN;
        return as;
    }

    void on_awake() override {
        model = Model::load_from_file_shared("./res/models/A_full.fbx");
        printf("size %lld\n", model->mdt.size());
        u_diffuse_color = bgfx::createUniform("u_diffuse_color", bgfx::UniformType::Vec4);

        program = std::make_shared<Shader>("./res/shaders/vs_unlit.bin", "./res/shaders/fs_unlit.bin");
    }

    void on_start() override {
        // add entities to scene
        auto teapot = scene.create();
        scene.emplace<Transform>(teapot);

        camera = scene.create();
        scene.emplace<Camera>(camera, Camera::perspective(glm::radians(60.f), (float)Screen::width() / Screen::height(), .1f, 300.f));
        scene.emplace<Transform>(camera, Transform::look_at(glm::vec3(-15, 15, -20), glm::vec3(0, 0, 0)));

        walk_speed   = 20.0f;
        rotate_speed = 30.0f;
    }

    void on_update() override {
        process_scene_input();
    }

    void on_render() override {
        // get camera info
        glm::mat4 view = scene.get<Transform>(camera).view_matrix();
        glm::mat4 proj = scene.get<Camera>(camera).matrix();
        bgfx::setViewTransform(Gfx::main_view(), glm::value_ptr(view), glm::value_ptr(proj));
        bgfx::setViewRect(Gfx::main_view(), 0, 0, Screen::draw_width(), Screen::draw_height());

        // render entities in scene
        // todo only render entities with RenderComponent
        auto teapots = scene.view<const Transform>(entt::exclude<Camera>);
        teapots.each([&](const Transform& trans) {
            // ...
        });

        for (const auto& mdt : model->mdt) {
            bgfx::setTransform(glm::value_ptr(mdt.transform));
            bgfx::setVertexBuffer(0, mdt.vbh);
            bgfx::setIndexBuffer(mdt.ibh);
            bgfx::setUniform(u_diffuse_color, glm::value_ptr(mdt.diffuse));
            bgfx::setState(BGFX_STATE_WRITE_RGB
                           | BGFX_STATE_WRITE_Z
                           | BGFX_STATE_DEPTH_TEST_LESS); // don't cull since model is corrupt
            bgfx::submit(Gfx::main_view(), program->handle());
        }
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
                switch (resolve) {
                case 0:
                    Screen::set_size(800, 600);
                    break;
                case 1:
                    Screen::set_size(1280, 720);
                    break;
                case 2:
                    Screen::set_size(1440, 960);
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

    void process_scene_input() {
        // use left ALT to switch between scene and gui
        if (Input::key_pressed(KeyCode::LAlt)) {
            gui_capture_input = !gui_capture_input;
            Gui::set_capture_keyboard(gui_capture_input);
            Gui::set_capture_mouse(gui_capture_input);
            Screen::set_relative_cursor(!gui_capture_input);
        }

        if (Input::key_pressed(KeyCode::Escape)) {
            request_quit();
            return;
        }

        if (gui_capture_input)
            return;

        // update camera
        Transform& trans = scene.get<Transform>(camera);
        glm::vec3 dir    = glm::zero<glm::vec3>();
        if (Input::key_repeat(KeyCode::W)) {
            dir += Transform::FORWARD;
        }
        if (Input::key_repeat(KeyCode::S)) {
            dir += Transform::BACK;
        }
        if (Input::key_repeat(KeyCode::A)) {
            dir += Transform::LEFT;
        }
        if (Input::key_repeat(KeyCode::D)) {
            dir += Transform::RIGHT;
        }
        if (Input::key_repeat(KeyCode::E)) {
            dir += Transform::UP;
        }
        if (Input::key_repeat(KeyCode::Q)) {
            dir += Transform::DOWN;
        }

        if (!glm::all(glm::epsilonEqual(dir, glm::zero<glm::vec3>(), 0.0001f))) {
            dir = glm::normalize(dir);
            trans.translate_in_local(dir * Time::delta() * walk_speed);
        }

        glm::ivec2 mouse = Input::mouse_pos_delta();
        if (mouse.x != 0 || mouse.y != 0) {
            glm::quat rotation = glm::identity<glm::quat>();
            rotation           = glm::rotate(rotation, glm::radians(mouse.x * Time::delta() * rotate_speed), Transform::UP);
            rotation           = glm::rotate(rotation, glm::radians(mouse.y * Time::delta() * rotate_speed), trans.right());
            trans.rotate(rotation);
        }
    }

    bool gui_capture_input = true;
    float walk_speed       = 1.0f;
    float rotate_speed     = 1.0f;
    std::shared_ptr<Model> model;
    std::shared_ptr<Shader> program;
    bgfx::UniformHandle u_diffuse_color = BGFX_INVALID_HANDLE;

    // todo put these into base class
    entt::registry scene;
    entt::entity camera;
};

LAUNCH_APP(Demo)
