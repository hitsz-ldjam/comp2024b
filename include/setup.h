#pragma once
#include "types.h"
#include <string>

struct AppSetup {
    std::string title = u8"App";
    bool centered     = true;
    i32 x             = 0;
    i32 y             = 0;
    i32 width         = 800;
    i32 height        = 600;
    u32 flags         = 0;
};