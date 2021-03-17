#pragma once
#include "types.h"
#include <glm/ext/vector_int2.hpp>

enum class KeyCode : u32 {
    Unknown,

    // Character Keys
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    // Arrow Keys
    Left,
    Up,
    Right,
    Down,

    // Numbers
    /**< Numbers above the alphabet characters, below the functions keys on a traditional QWERTY keyboard */
    Num_1,
    Num_2,
    Num_3,
    Num_4,
    Num_5,
    Num_6,
    Num_7,
    Num_8,
    Num_9,
    Num_0,

    /**< These keys are on numpad */
    Np_1,
    Np_2,
    Np_3,
    Np_4,
    Np_5,
    Np_6,
    Np_7,
    Np_8,
    Np_9,
    Np_0,
    Np_Add,
    Np_Minus,
    Np_Multiply,
    Np_Divide,
    Np_Enter,
    Np_Decimal,

    Backspace,
    Tab,
    Enter,
    Pause,
    Numlock,
    ScrollLock,
    Capslock,
    Escape,
    Space,

    Insert,
    Delete,
    Home,
    End,
    PageUp,
    PageDown,

    // Function Keys
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,

    LCtrl,
    RCtrl,
    LShift,
    RShift,
    LAlt,
    RAlt,
    LGUI,
    RGUI,

    Semicolon,
    Equals,
    Comma,
    Minus,
    Period,
    Slash,
    Backquote,
    LeftBracket,
    RightBracket,
    Backslash,
    Quote,

    EnumBegin = A,
    EnumEnd   = Quote + 1,
    EnumCount = EnumEnd - EnumBegin,
};

class Input {
public:
    static bool init();
    static void quit();
    static void new_frame();
    static bool process_event(const union SDL_Event& event);

    static bool key_repeat(KeyCode key);
    static bool key_pressed(KeyCode key);
    static bool key_released(KeyCode key);
    static bool any_key_repeat();
    static bool any_key_pressed();
    static bool any_key_released();
    static glm::ivec2 mouse_pos();
    static glm::ivec2 mouse_pos_delta();
    static glm::ivec2 mouse_wheel();
    static bool mouse_button_pressed(u32 btn);
    static bool mouse_button_released(u32 btn);
    static bool mouse_button_repeat(u32 btn);
    static bool any_mouse_button_pressed();
    static bool any_mouse_button_released();
    static bool any_mouse_button_repeat();
};