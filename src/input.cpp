#include "input.h"
#include <SDL2/SDL_events.h>

static const u32 MOUSE_BUTTON_COUNT = 5;

#define KEYCODE_TO_INDEX(key) (u32(key) - u32(KeyCode::EnumBegin))

KeyCode map_keycode(SDL_Scancode key) {
    switch (key) {
    case SDL_SCANCODE_A:
        return KeyCode::A;
    case SDL_SCANCODE_B:
        return KeyCode::B;
    case SDL_SCANCODE_C:
        return KeyCode::C;
    case SDL_SCANCODE_D:
        return KeyCode::D;
    case SDL_SCANCODE_E:
        return KeyCode::E;
    case SDL_SCANCODE_F:
        return KeyCode::F;
    case SDL_SCANCODE_G:
        return KeyCode::G;
    case SDL_SCANCODE_H:
        return KeyCode::H;
    case SDL_SCANCODE_I:
        return KeyCode::I;
    case SDL_SCANCODE_J:
        return KeyCode::J;
    case SDL_SCANCODE_K:
        return KeyCode::K;
    case SDL_SCANCODE_L:
        return KeyCode::L;
    case SDL_SCANCODE_M:
        return KeyCode::M;
    case SDL_SCANCODE_N:
        return KeyCode::N;
    case SDL_SCANCODE_O:
        return KeyCode::O;
    case SDL_SCANCODE_P:
        return KeyCode::P;
    case SDL_SCANCODE_Q:
        return KeyCode::Q;
    case SDL_SCANCODE_R:
        return KeyCode::R;
    case SDL_SCANCODE_S:
        return KeyCode::S;
    case SDL_SCANCODE_T:
        return KeyCode::T;
    case SDL_SCANCODE_U:
        return KeyCode::U;
    case SDL_SCANCODE_V:
        return KeyCode::V;
    case SDL_SCANCODE_W:
        return KeyCode::W;
    case SDL_SCANCODE_X:
        return KeyCode::X;
    case SDL_SCANCODE_Y:
        return KeyCode::Y;
    case SDL_SCANCODE_Z:
        return KeyCode::Z;

    case SDL_SCANCODE_LEFT:
        return KeyCode::Left;
    case SDL_SCANCODE_UP:
        return KeyCode::Up;
    case SDL_SCANCODE_RIGHT:
        return KeyCode::Right;
    case SDL_SCANCODE_DOWN:
        return KeyCode::Down;

    case SDL_SCANCODE_1:
        return KeyCode::Num_1;
    case SDL_SCANCODE_2:
        return KeyCode::Num_2;
    case SDL_SCANCODE_3:
        return KeyCode::Num_3;
    case SDL_SCANCODE_4:
        return KeyCode::Num_4;
    case SDL_SCANCODE_5:
        return KeyCode::Num_5;
    case SDL_SCANCODE_6:
        return KeyCode::Num_6;
    case SDL_SCANCODE_7:
        return KeyCode::Num_7;
    case SDL_SCANCODE_8:
        return KeyCode::Num_8;
    case SDL_SCANCODE_9:
        return KeyCode::Num_9;
    case SDL_SCANCODE_0:
        return KeyCode::Num_0;

    case SDL_SCANCODE_KP_1:
        return KeyCode::Np_1;
    case SDL_SCANCODE_KP_2:
        return KeyCode::Np_2;
    case SDL_SCANCODE_KP_3:
        return KeyCode::Np_3;
    case SDL_SCANCODE_KP_4:
        return KeyCode::Np_4;
    case SDL_SCANCODE_KP_5:
        return KeyCode::Np_5;
    case SDL_SCANCODE_KP_6:
        return KeyCode::Np_6;
    case SDL_SCANCODE_KP_7:
        return KeyCode::Np_7;
    case SDL_SCANCODE_KP_8:
        return KeyCode::Np_8;
    case SDL_SCANCODE_KP_9:
        return KeyCode::Np_9;
    case SDL_SCANCODE_KP_0:
        return KeyCode::Np_0;
    case SDL_SCANCODE_KP_PLUS:
        return KeyCode::Np_Add;
    case SDL_SCANCODE_KP_MINUS:
        return KeyCode::Np_Minus;
    case SDL_SCANCODE_KP_MULTIPLY:
        return KeyCode::Np_Multiply;
    case SDL_SCANCODE_KP_DIVIDE:
        return KeyCode::Np_Divide;
    case SDL_SCANCODE_KP_ENTER:
        return KeyCode::Np_Enter;
    case SDL_SCANCODE_KP_PERIOD:
        return KeyCode::Np_Decimal;

    case SDL_SCANCODE_BACKSPACE:
        return KeyCode::Backspace;
    case SDL_SCANCODE_TAB:
        return KeyCode::Tab;
    case SDL_SCANCODE_RETURN:
        return KeyCode::Enter;
    case SDL_SCANCODE_PAUSE:
        return KeyCode::Pause;
    case SDL_SCANCODE_NUMLOCKCLEAR:
        return KeyCode::Numlock;
    case SDL_SCANCODE_SCROLLLOCK:
        return KeyCode::ScrollLock;
    case SDL_SCANCODE_CAPSLOCK:
        return KeyCode::Capslock;
    case SDL_SCANCODE_ESCAPE:
        return KeyCode::Escape;
    case SDL_SCANCODE_SPACE:
        return KeyCode::Space;

    case SDL_SCANCODE_INSERT:
        return KeyCode::Insert;
    case SDL_SCANCODE_DELETE:
        return KeyCode::Delete;
    case SDL_SCANCODE_HOME:
        return KeyCode::Home;
    case SDL_SCANCODE_END:
        return KeyCode::End;
    case SDL_SCANCODE_PAGEUP:
        return KeyCode::PageUp;
    case SDL_SCANCODE_PAGEDOWN:
        return KeyCode::PageDown;

    case SDL_SCANCODE_F1:
        return KeyCode::F1;
    case SDL_SCANCODE_F2:
        return KeyCode::F2;
    case SDL_SCANCODE_F3:
        return KeyCode::F3;
    case SDL_SCANCODE_F4:
        return KeyCode::F4;
    case SDL_SCANCODE_F5:
        return KeyCode::F5;
    case SDL_SCANCODE_F6:
        return KeyCode::F6;
    case SDL_SCANCODE_F7:
        return KeyCode::F7;
    case SDL_SCANCODE_F8:
        return KeyCode::F8;
    case SDL_SCANCODE_F9:
        return KeyCode::F9;
    case SDL_SCANCODE_F10:
        return KeyCode::F10;
    case SDL_SCANCODE_F11:
        return KeyCode::F11;
    case SDL_SCANCODE_F12:
        return KeyCode::F12;

    case SDL_SCANCODE_LCTRL:
        return KeyCode::LCtrl;
    case SDL_SCANCODE_RCTRL:
        return KeyCode::RCtrl;
    case SDL_SCANCODE_LSHIFT:
        return KeyCode::LShift;
    case SDL_SCANCODE_RSHIFT:
        return KeyCode::RShift;
    case SDL_SCANCODE_LALT:
        return KeyCode::LAlt;
    case SDL_SCANCODE_RALT:
        return KeyCode::RAlt;
    case SDL_SCANCODE_LGUI:
        return KeyCode::LGUI;
    case SDL_SCANCODE_RGUI:
        return KeyCode::RGUI;

    case SDL_SCANCODE_SEMICOLON:
        return KeyCode::Semicolon;
    case SDL_SCANCODE_EQUALS:
        return KeyCode::Equals;
    case SDL_SCANCODE_COMMA:
        return KeyCode::Comma;
    case SDL_SCANCODE_MINUS:
        return KeyCode::Minus;
    case SDL_SCANCODE_PERIOD:
        return KeyCode::Period;
    case SDL_SCANCODE_SLASH:
        return KeyCode::Slash;
    case SDL_SCANCODE_GRAVE:
        return KeyCode::Backquote;
    case SDL_SCANCODE_LEFTBRACKET:
        return KeyCode::LeftBracket;
    case SDL_SCANCODE_RIGHTBRACKET:
        return KeyCode::RightBracket;
    case SDL_SCANCODE_BACKSLASH:
        return KeyCode::Backslash;
    case SDL_SCANCODE_APOSTROPHE:
        return KeyCode::Quote;

    default:
        return KeyCode::Unknown;
    }
}

enum KeyState : u8 {
    KS_Pressed     = 0x01,
    KS_Released    = 0x02,
    KS_Repeat      = 0x04
};

struct InputImpl {

    InputImpl()
        : mouse_pos(0),
          mouse_delta_pos(0),
          wheel_state(0) {
        memset(key_states, 0, sizeof(key_states));
        memset(mbtn_states, 0, sizeof(mbtn_states));
    }

    void update_key(KeyCode key, KeyState state) {
        u32 idx = KEYCODE_TO_INDEX(key);
        if (state == KS_Pressed && !(key_states[idx] & KS_Repeat)) {
            key_states[idx] = KS_Pressed | KS_Repeat;
            any_key_state = KS_Pressed;
            ++reapt_key_count;
        }
        else if (state == KS_Released) {
            key_states[idx] = KS_Released;
            any_key_state = KS_Released;
            --reapt_key_count;
            assert(reapt_key_count >= 0);
        }
    }

    void update_mouse_button(u32 btn, KeyState state) {
        if (btn >= MOUSE_BUTTON_COUNT)
            return;

        if (state == KS_Pressed && !(mbtn_states[btn] & KS_Repeat)) {
            mbtn_states[btn] = KS_Pressed | KS_Repeat;
            any_mbtn_state = KS_Pressed;
            ++repeat_mbtn_count;
        }
        else if (state == KS_Released) {
            mbtn_states[btn] = KS_Released;
            any_mbtn_state = KS_Released;
            --repeat_mbtn_count;
            assert(repeat_mbtn_count >= 0);
        }
    }

    void new_frame() {
        for (u32 i = 0; i < (u32)KeyCode::EnumCount; ++i) {
            key_states[i] &= ~(KS_Pressed | KS_Released);
        }
        any_key_state = 0;

        for (u32 i = 0; i < MOUSE_BUTTON_COUNT; ++i) {
            mbtn_states[i] &= ~(KS_Pressed | KS_Released);
        }
        any_mbtn_state = 0;

        wheel_state = glm::ivec2(0);
        mouse_delta_pos = glm::ivec2(0);
    }

    u8 key_states[(u32)KeyCode::EnumCount];
    u8 any_key_state = 0;
    u8 reapt_key_count = 0;

    u8 mbtn_states[MOUSE_BUTTON_COUNT];
    u8 any_mbtn_state = 0;
    u8 repeat_mbtn_count = 0;
    glm::ivec2 mouse_pos;
    glm::ivec2 mouse_delta_pos;
    glm::ivec2 wheel_state;
};

static InputImpl* s_input_impl = nullptr;

bool Input::init() {
    assert(s_input_impl == nullptr && "input is initialized twice");
    s_input_impl = new InputImpl();
    return true;
}

void Input::quit() {
    delete s_input_impl;
    s_input_impl = nullptr;
}

void Input::new_frame() {
    s_input_impl->new_frame();
}

bool Input::process_event(const SDL_Event& event) {
    switch (event.type) {
    case SDL_KEYDOWN:
        s_input_impl->update_key(map_keycode(event.key.keysym.scancode), KS_Pressed);
        break;

    case SDL_KEYUP:
        s_input_impl->update_key(map_keycode(event.key.keysym.scancode), KS_Released);
        break;

    case SDL_MOUSEBUTTONDOWN:
        s_input_impl->update_mouse_button(event.button.button - 1, KS_Pressed);
        break;

    case SDL_MOUSEBUTTONUP:
        s_input_impl->update_mouse_button(event.button.button - 1, KS_Released);
        break;

    case SDL_MOUSEWHEEL:
        s_input_impl->wheel_state = glm::ivec2(event.wheel.x, event.wheel.y);
        break;

    case SDL_MOUSEMOTION:
        s_input_impl->mouse_pos = glm::ivec2(event.motion.x, event.motion.y);
        s_input_impl->mouse_delta_pos = glm::ivec2(event.motion.xrel, event.motion.yrel);
        break;
    }

    return false; // not cosuming the event
}

bool Input::key_repeat(KeyCode key) {
    return s_input_impl->key_states[KEYCODE_TO_INDEX(key)] & KS_Repeat;
}

bool Input::key_pressed(KeyCode key) {
    return s_input_impl->key_states[KEYCODE_TO_INDEX(key)] & KS_Pressed;
}

bool Input::key_released(KeyCode key) {
    return s_input_impl->key_states[KEYCODE_TO_INDEX(key)] & KS_Released;
}

bool Input::any_key_repeat() {
    return s_input_impl->reapt_key_count != 0;
}

bool Input::any_key_pressed() {
    return s_input_impl->any_key_state & KS_Pressed;
}

bool Input::any_key_released() {
    return s_input_impl->any_key_state & KS_Released;
}

glm::ivec2 Input::mouse_pos() {
    return s_input_impl->mouse_pos;
}

glm::ivec2 Input::mouse_pos_delta() {
    return s_input_impl->mouse_delta_pos;
}

glm::ivec2 Input::mouse_wheel() {
    return s_input_impl->wheel_state;
}

bool Input::mouse_button_pressed(u32 btn) {
    if (btn >= MOUSE_BUTTON_COUNT)
        return false;

    return s_input_impl->mbtn_states[btn] & KS_Pressed;
}

bool Input::mouse_button_released(u32 btn) {
    if (btn >= MOUSE_BUTTON_COUNT)
        return false;

    return s_input_impl->mbtn_states[btn] & KS_Released;
}

bool Input::mouse_button_repeat(u32 btn) {
    if (btn >= MOUSE_BUTTON_COUNT)
        return false;

    return s_input_impl->mbtn_states[btn] & KS_Repeat;
}

bool Input::any_mouse_button_pressed() {
    return s_input_impl->any_mbtn_state & KS_Pressed;
}

bool Input::any_mouse_button_released() {
    return s_input_impl->any_mbtn_state & KS_Released;
}

bool Input::any_mouse_button_repeat() {
    return s_input_impl->repeat_mbtn_count != 0;
}
