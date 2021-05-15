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

struct ScreenQuadVertex {
    glm::vec3 pos;
    glm::vec2 uv;

    static const bgfx::VertexLayout& layout() {
        static bgfx::VertexLayout s_layout;
        static bool flag = true;

        if (flag) {
            s_layout.begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .end();

            flag = false;
        }

        return s_layout;
    }
};

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

        const uint64_t sampler_flags = 0
                                       | BGFX_SAMPLER_MIN_POINT
                                       | BGFX_SAMPLER_MAG_POINT
                                       | BGFX_SAMPLER_MIP_POINT
                                       | BGFX_SAMPLER_U_CLAMP
                                       | BGFX_SAMPLER_V_CLAMP;

        bgfx::TextureHandle color = bgfx::createTexture2D(Screen::draw_width(),
                                                          Screen::draw_height(),
                                                          false,
                                                          1,
                                                          bgfx::TextureFormat::RGBA8,
                                                          BGFX_TEXTURE_RT | sampler_flags);

        bgfx::TextureFormat::Enum depth_format = bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::D32F, BGFX_TEXTURE_RT | sampler_flags)
                                                     ? bgfx::TextureFormat::D32F
                                                     : bgfx::TextureFormat::D24;

        bgfx::TextureHandle depth = bgfx::createTexture2D(Screen::draw_width(),
                                                          Screen::draw_height(),
                                                          false,
                                                          1,
                                                          depth_format,
                                                          BGFX_TEXTURE_RT | sampler_flags);

        bgfx::Attachment attach[2];
        attach[0].init(color);
        attach[1].init(depth, bgfx::Access::ReadWrite);

        main_fb = bgfx::createFrameBuffer(std::size(attach), attach, true);

        pe_color  = bgfx::createUniform("s_color", bgfx::UniformType::Sampler);
        pe_depth  = bgfx::createUniform("s_depth", bgfx::UniformType::Sampler);
        pe_params = bgfx::createUniform("u_params", bgfx::UniformType::Vec4, 4);
        pe_shader = std::make_shared<Shader>("./res/shaders/vs_fog.bin", "./res/shaders/fs_fog.bin");
    }

    void on_start() override {
        // add entities to scene
        auto teapot = scene.create();
        scene.emplace<Transform>(teapot);
        auto& render_comp   = scene.emplace<RenderComponent>(teapot, RenderComponent(model, program));
        render_comp.uniform = u_diffuse_color;

        camera_entity = scene.create();
        scene.emplace<Camera>(camera_entity, Camera::perspective(glm::radians(60.f), (float)Screen::width() / Screen::height(), .1f, 300.f));
        scene.emplace<Transform>(camera_entity, Transform::look_at(glm::vec3(-15, 15, -20), glm::vec3(0, 0, 0)));
        scene.emplace<CameraControlData>(camera_entity, CameraControlData{ 20.0f, 30.0f });
    }

    void on_update() override {
        if (!process_scene_input())
            return;
        Systems::camera_control(scene);
    }

    void on_render() override {
        bgfx::setViewFrameBuffer(Gfx::main_view(), main_fb);
        Systems::rendering(scene);

        auto scene_color = bgfx::getTexture(main_fb, 0);
        auto scene_depth = bgfx::getTexture(main_fb, 1);

        // volumetric fog rendering
        Transform& trans = scene.get<Transform>(camera_entity);
        Camera& camera   = scene.get<Camera>(camera_entity);
        glm::mat4 view   = trans.view_matrix();
        glm::mat4 proj   = camera.matrix();

        bgfx::setViewTransform(Gfx::pe_view(), glm::value_ptr(view), glm::value_ptr(proj));
        bgfx::setViewRect(Gfx::pe_view(), 0, 0, Screen::draw_width(), Screen::draw_height());
        bgfx::setViewClear(Gfx::pe_view(), BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0);

        glm::vec4 params[4]; // pass parameters to shader, see fs_fog.sc file
        params[0] = glm::vec4(trans.position, 0);
        params[1] = glm::vec4(-10, -10, -10, 0);
        params[2] = glm::vec4(10, 10, 10, 0);
        bgfx::setUniform(pe_params, params, std::size(params));

        // draw screen quad
        bgfx::setVertexCount(3);
        bgfx::setState(BGFX_STATE_WRITE_RGB
                           | BGFX_STATE_DEPTH_TEST_ALWAYS
                           | BGFX_STATE_CULL_CW
                           | BGFX_STATE_BLEND_ALPHA,
                       0);
        bgfx::setTexture(0, pe_color, scene_color);
        bgfx::setTexture(1, pe_depth, scene_depth);
        bgfx::submit(Gfx::pe_view(), pe_shader->handle());
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

        pe_shader.reset();
        bgfx::destroy(pe_color);
        bgfx::destroy(pe_depth);
        bgfx::destroy(pe_params);

        program.reset();
        bgfx::destroy(u_diffuse_color);
        bgfx::destroy(main_fb);
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
                Camera& c = scene.get<Camera>(camera_entity);
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

    bgfx::FrameBufferHandle main_fb;
    bgfx::UniformHandle pe_color;
    bgfx::UniformHandle pe_depth;
    bgfx::UniformHandle pe_params;
    std::shared_ptr<Shader> pe_shader;

    // todo put these into base class
    entt::registry scene;
    entt::entity camera_entity;
};

LAUNCH_APP(Demo)
