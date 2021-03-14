#include "gui.h"
#include <filesystem>
#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL2/SDL.h>

#include "embedded/shaders/vs_ocornut_imgui.bin.h"
#include "embedded/shaders/fs_ocornut_imgui.bin.h"
#include "embedded/shaders/vs_imgui_image.bin.h"
#include "embedded/shaders/fs_imgui_image.bin.h"

static const bgfx::EmbeddedShader EMBEDDED_SHADER[] = {
    BGFX_EMBEDDED_SHADER(vs_ocornut_imgui),
    BGFX_EMBEDDED_SHADER(fs_ocornut_imgui),
    BGFX_EMBEDDED_SHADER(vs_imgui_image),
    BGFX_EMBEDDED_SHADER(fs_imgui_image),

    BGFX_EMBEDDED_SHADER_END()
};

struct GuiImpl {
    bgfx::ProgramHandle shader            = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle image_shader      = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle image_lod_enabled = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle sampler           = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle texture           = BGFX_INVALID_HANDLE;
    bgfx::VertexLayout vertex_layout      = bgfx::VertexLayout();
    ImFont* font                          = nullptr;
    // todo add cursor support
};

static GuiImpl* GUI_IMPL = nullptr;

void Gui::begin_frame(i32 width, i32 height, float w_scale, float h_scale) {
    ImGuiIO& io    = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)width, (float)height);
    if (w_scale > 0.0f && h_scale > 0.0f) {
        io.DisplayFramebufferScale = ImVec2(w_scale, h_scale);
    }
    ImGui::NewFrame();
}

void Gui::end_frame() {
    ImGui::EndFrame();
}

void Gui::render() {
    ImGui::Render();
    ImGuiIO& io            = ImGui::GetIO();
    const float width      = io.DisplaySize.x;
    const float height     = io.DisplaySize.y;
    ImDrawData* draw_data  = ImGui::GetDrawData();
    const bgfx::Caps* caps = bgfx::getCaps();
    {
        glm::mat4 mat;
        if (caps->homogeneousDepth) {
            mat = glm::orthoLH_NO(0.0f, width, height, 0.0f, 0.0f, 100.0f);
        }
        else {
            mat = glm::orthoLH_ZO(0.0f, width, height, 0.0f, 0.0f, 100.0f);
        }
        bgfx::setViewTransform(0, nullptr, glm::value_ptr(mat));
        bgfx::setViewRect(0, 0, 0, (u16)width, (u16)height);
    }

    for (i32 i = 0; i < draw_data->CmdListsCount; ++i) {
        bgfx::TransientVertexBuffer tvb{};
        bgfx::TransientIndexBuffer tib{};

        const ImDrawList* draw_list = draw_data->CmdLists[i];
        uint32_t num_vertices       = (uint32_t)draw_list->VtxBuffer.size();
        uint32_t num_indices        = (uint32_t)draw_list->IdxBuffer.size();

        // check buffer capacity
        if (!(num_vertices == bgfx::getAvailTransientVertexBuffer(num_vertices, GUI_IMPL->vertex_layout)
              && (num_indices == 0 || num_indices == bgfx::getAvailTransientIndexBuffer(num_indices)))) {
            break;
        }

        bgfx::allocTransientVertexBuffer(&tvb, num_vertices, GUI_IMPL->vertex_layout);
        bgfx::allocTransientIndexBuffer(&tib, num_indices, sizeof(ImDrawIdx) == 4);

        ImDrawVert* verts  = (ImDrawVert*)tvb.data;
        ImDrawIdx* indices = (ImDrawIdx*)tib.data;
        memcpy(verts, draw_list->VtxBuffer.begin(), num_vertices * sizeof(ImDrawVert));
        memcpy(indices, draw_list->IdxBuffer.begin(), num_indices * sizeof(ImDrawIdx));

        u32 offset = 0;
        for (const ImDrawCmd& cmd : draw_list->CmdBuffer) {
            if (cmd.UserCallback) {
                cmd.UserCallback(draw_list, &cmd);
            }
            else if (cmd.ElemCount != 0) {
                u64 state = 0
                            | BGFX_STATE_WRITE_RGB
                            | BGFX_STATE_WRITE_A
                            | BGFX_STATE_MSAA;
                bgfx::TextureHandle tex    = GUI_IMPL->texture;
                bgfx::ProgramHandle shader = GUI_IMPL->shader;

                if (cmd.TextureId != NULL) {
                    union {
                        ImTextureID ptr;
                        struct {
                            bgfx::TextureHandle handle;
                            uint8_t flags;
                            uint8_t mip;
                        } s;
                    } texture = { cmd.TextureId };

                    // todo process blend state

                    tex = texture.s.handle;
                    if (0 != texture.s.mip) {
                        const float lodEnabled[4] = { float(texture.s.mip), 1.0f, 0.0f, 0.0f };
                        bgfx::setUniform(GUI_IMPL->image_lod_enabled, lodEnabled);
                        shader = GUI_IMPL->image_shader;
                    }
                }
                else {
                    state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
                }

                const u16 xx = u16(bx::max(cmd.ClipRect.x, 0.0f));
                const u16 yy = u16(bx::max(cmd.ClipRect.y, 0.0f));
                bgfx::setScissor(xx,
                                 yy,
                                 u16(bx::min(cmd.ClipRect.z, 65535.0f) - xx),
                                 u16(bx::min(cmd.ClipRect.w, 65535.0f) - yy));

                bgfx::setState(state);
                bgfx::setTexture(0, GUI_IMPL->sampler, tex);
                bgfx::setVertexBuffer(0, &tvb, 0, num_vertices);
                bgfx::setIndexBuffer(&tib, offset, cmd.ElemCount);
                bgfx::submit(0, shader);
            }
            offset += cmd.ElemCount;
        }
    }
}

bool Gui::init() {
    assert(GUI_IMPL == nullptr && "gui is initialized twice");
    GUI_IMPL = new GuiImpl();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO& io                          = ImGui::GetIO();
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // map keyboard
    io.KeyMap[ImGuiKey_Tab]         = SDL_SCANCODE_TAB;
    io.KeyMap[ImGuiKey_LeftArrow]   = SDL_SCANCODE_LEFT;
    io.KeyMap[ImGuiKey_RightArrow]  = SDL_SCANCODE_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow]     = SDL_SCANCODE_UP;
    io.KeyMap[ImGuiKey_DownArrow]   = SDL_SCANCODE_DOWN;
    io.KeyMap[ImGuiKey_PageUp]      = SDL_SCANCODE_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown]    = SDL_SCANCODE_PAGEDOWN;
    io.KeyMap[ImGuiKey_Home]        = SDL_SCANCODE_HOME;
    io.KeyMap[ImGuiKey_End]         = SDL_SCANCODE_END;
    io.KeyMap[ImGuiKey_Insert]      = SDL_SCANCODE_INSERT;
    io.KeyMap[ImGuiKey_Delete]      = SDL_SCANCODE_DELETE;
    io.KeyMap[ImGuiKey_Backspace]   = SDL_SCANCODE_BACKSPACE;
    io.KeyMap[ImGuiKey_Space]       = SDL_SCANCODE_SPACE;
    io.KeyMap[ImGuiKey_Enter]       = SDL_SCANCODE_RETURN;
    io.KeyMap[ImGuiKey_KeyPadEnter] = SDL_SCANCODE_KP_ENTER;
    io.KeyMap[ImGuiKey_Escape]      = SDL_SCANCODE_ESCAPE;
    io.KeyMap[ImGuiKey_A]           = SDL_SCANCODE_A;
    io.KeyMap[ImGuiKey_C]           = SDL_SCANCODE_C;
    io.KeyMap[ImGuiKey_V]           = SDL_SCANCODE_V;
    io.KeyMap[ImGuiKey_X]           = SDL_SCANCODE_X;
    io.KeyMap[ImGuiKey_Y]           = SDL_SCANCODE_Y;
    io.KeyMap[ImGuiKey_Z]           = SDL_SCANCODE_Z;

    // init shader
    {
        bgfx::RendererType::Enum type = bgfx::getRendererType();
        GUI_IMPL->shader              = bgfx::createProgram(
            bgfx::createEmbeddedShader(EMBEDDED_SHADER, type, "vs_ocornut_imgui"),
            bgfx::createEmbeddedShader(EMBEDDED_SHADER, type, "fs_ocornut_imgui"),
            true);

        GUI_IMPL->image_lod_enabled = bgfx::createUniform("image_lod_enabled", bgfx::UniformType::Vec4);

        GUI_IMPL->image_shader = bgfx::createProgram(
            bgfx::createEmbeddedShader(EMBEDDED_SHADER, type, "vs_imgui_image"),
            bgfx::createEmbeddedShader(EMBEDDED_SHADER, type, "fs_imgui_image"),
            true);

        GUI_IMPL->sampler = bgfx::createUniform("s_tex", bgfx::UniformType::Sampler);

        GUI_IMPL->vertex_layout
            .begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
    }

    // load font
    namespace fs = std::filesystem;
    // todo make this cutomizable
    std::string font_path = "res/fonts/Roboto-Regular.ttf";
    {
        const ImWchar* ranges = io.Fonts->GetGlyphRangesCyrillic();
        ImFontConfig config;
        config.FontDataOwnedByAtlas = false;
        config.MergeMode            = false;

        if (fs::exists(font_path)) {
            GUI_IMPL->font = io.Fonts->AddFontFromFileTTF(font_path.c_str(), 16, &config, ranges);
        } else {
            GUI_IMPL->font              = io.Fonts->AddFontDefault(&config);
        }

        uint8_t* data;
        int32_t width;
        int32_t height;
        io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

        GUI_IMPL->texture = bgfx::createTexture2D(
            (uint16_t)width,
            (uint16_t)height,
            false,
            1,
            bgfx::TextureFormat::BGRA8,
            0,
            bgfx::copy(data, width * height * 4));

        bgfx::setName(GUI_IMPL->texture, "gui_font_texture");
    }

    return true;
}

void Gui::quit() {
    ImGui::DestroyContext();
    // todo check this
    // bgfx::destroy(GUI_IMPL->shader);
    // bgfx::destroy(GUI_IMPL->image_shader);
    // bgfx::destroy(GUI_IMPL->image_lod_enabled);
    // bgfx::destroy(GUI_IMPL->texture);
    // bgfx::destroy(GUI_IMPL->sampler);
    delete GUI_IMPL;
    GUI_IMPL = nullptr;
}

void Gui::process_event(const SDL_Event& event) {
    ImGuiIO& io = ImGui::GetIO();
    switch (event.type) {
    case SDL_MOUSEWHEEL:
        if (event.wheel.x > 0)
            io.MouseWheelH += 1;
        if (event.wheel.x < 0)
            io.MouseWheelH -= 1;
        if (event.wheel.y > 0)
            io.MouseWheel += 1;
        if (event.wheel.y < 0)
            io.MouseWheel -= 1;
        break;

    case SDL_MOUSEBUTTONDOWN:
        switch (event.button.button) {
        case SDL_BUTTON_LEFT: io.MouseDown[0] = true; break;
        case SDL_BUTTON_MIDDLE: io.MouseDown[1] = true; break;
        case SDL_BUTTON_RIGHT: io.MouseDown[2] = true; break;
        case SDL_BUTTON_X1: io.MouseDown[3] = true; break;
        case SDL_BUTTON_X2: io.MouseDown[4] = true; break;
        default: break;
        }
        break;

    case SDL_MOUSEBUTTONUP:
        switch (event.button.button) {
        case SDL_BUTTON_LEFT: io.MouseDown[0] = false; break;
        case SDL_BUTTON_MIDDLE: io.MouseDown[1] = false; break;
        case SDL_BUTTON_RIGHT: io.MouseDown[2] = false; break;
        case SDL_BUTTON_X1: io.MouseDown[3] = false; break;
        case SDL_BUTTON_X2: io.MouseDown[4] = false; break;
        default: break;
        }
        break;

    case SDL_MOUSEMOTION:
        io.MousePos = ImVec2(event.motion.x, event.motion.y);
        break;

    case SDL_KEYDOWN:
        io.KeysDown[event.key.keysym.scancode] = true;
        if (event.key.keysym.mod & KMOD_CTRL)
            io.KeyCtrl = true;
        if (event.key.keysym.mod & KMOD_SHIFT)
            io.KeyShift = true;
        if (event.key.keysym.mod & KMOD_ALT)
            io.KeyAlt = true;
        if (event.key.keysym.mod & KMOD_GUI)
            io.KeySuper = true;
        break;

    case SDL_KEYUP:
        io.KeysDown[event.key.keysym.scancode] = false;
        if (event.key.keysym.mod & KMOD_CTRL)
            io.KeyCtrl = false;
        if (event.key.keysym.mod & KMOD_SHIFT)
            io.KeyShift = false;
        if (event.key.keysym.mod & KMOD_ALT)
            io.KeyAlt = false;
        if (event.key.keysym.mod & KMOD_GUI)
            io.KeySuper = false;
        break;

    case SDL_TEXTINPUT:
        if (io.WantTextInput) {
            io.AddInputCharactersUTF8(event.text.text);
        }
        break;
    }
}

bool Gui::want_capture_mouse() {
    return ImGui::GetIO().WantCaptureMouse;
}

bool Gui::want_capture_keyboard() {
    return ImGui::GetIO().WantCaptureKeyboard;
}
