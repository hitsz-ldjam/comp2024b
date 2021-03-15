#pragma once

class Time {
public:
    static bool init();
    static void quit();
    static void tick();
    static float delta();
    static float real();
};