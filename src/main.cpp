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
#include "core/model.h"

#include "components/transform.h"

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
        model = Model::load_from_file_shared("./res/models/teapot.obj");
        if (bgfx::getRendererType() != bgfx::RendererType::Direct3D11)
            printf("It may work as well, perhaps.");

        // todo: move to Shader class
        auto load_shader = [](const std::string& filename) {
            std::ifstream file(filename, std::ios::binary | std::ios::ate);
            const auto filesize = file.tellg();
            file.seekg(0, std::ios::beg);
            const auto* mem = bgfx::alloc(filesize);
            file.read(reinterpret_cast<char*>(mem->data), filesize);
            file.close();
            auto shader = bgfx::createShader(mem);
            //bgfx::setName(shader, "debug name");
            return shader;
        };

        auto vert_sh = load_shader("./res/shaders/vs_unlit.bin");
        auto frag_sh = load_shader("./res/shaders/fs_unlit.bin");

        program = bgfx::createProgram(vert_sh, frag_sh, true);

        // add entities to scene
        auto teapot    = scene.create();
        auto& trans    = scene.emplace<Transform>(teapot);
        trans.position = glm::vec3(0);
        trans.rotation = glm::quat(1, 0, 0, 0);
        trans.scale    = glm::vec3(1);
    }

    void on_start() override {
    }

    void on_update() override {
        // todo: play with camera
        const auto view = glm::lookAt(glm::vec3(0, 10, -20),
                                      glm::vec3(0, 0, 0),
                                      glm::vec3(0, 1, 0));
        const auto proj = glm::perspective(glm::radians(60.f),
                                           (float)Screen::draw_width() / Screen::height(),
                                           .1f,
                                           100.f);
        bgfx::setViewTransform(Gfx::main_view(), glm::value_ptr(view), glm::value_ptr(proj));
        bgfx::setViewRect(Gfx::main_view(), 0, 0, Screen::draw_width(), Screen::draw_height());

        // update scene
        // todo use systems
        auto teapots = scene.view<Transform>();
        teapots.each([](Transform& trans) {
            trans.rotation = glm::rotate(trans.rotation, Time::delta(), glm::vec3(0, 1, 0));
        });
    }

    void on_render() override {
        auto teapots = scene.view<const Transform>();
        teapots.each([&](const Transform& trans) {
            auto mat = glm::mat4(1);
            mat = glm::translate(mat, trans.position);
            mat = mat * glm::toMat4(trans.rotation);
            mat = glm::scale(mat, trans.scale);
            bgfx::setTransform(glm::value_ptr(mat));

            bgfx::setVertexBuffer(0, model->vbh);
            bgfx::setIndexBuffer(model->ibh);
            bgfx::setState(BGFX_STATE_DEFAULT);
            bgfx::submit(Gfx::main_view(), program);
        });
    }

    void on_gui() override {
        ImGui::Begin("Test", nullptr, ImGuiWindowFlags_NoCollapse);
        {
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
                    case 0: Screen::set_size(800, 600); break;
                    case 1: Screen::set_size(1280, 720); break;
                    case 2: Screen::set_size(1440, 960); break;
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
        ImGui::End();

        // modal popup
        if (ask_quit)
            ImGui::OpenPopup("Quit?");

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Quit?", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Do you really want to quit?");
            if (ImGui::Button("Yes")) {
                do_quit  = true;
                ask_quit = false;
                request_quit();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("No")) {
                do_quit  = false;
                ask_quit = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    bool on_closing() override {
        ask_quit = true;
        return do_quit;
    }

    void on_quit() override {
        bgfx::destroy(program);
        model.reset();
    }

private:
    bool ask_quit = false;
    bool do_quit  = false;
    std::shared_ptr<Model> model;
    bgfx::ProgramHandle program{ bgfx::kInvalidHandle };

    // todo put these into base class
    entt::registry scene;
};

LAUNCH_APP(Demo)
