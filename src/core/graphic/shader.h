#pragma once

#include <bgfx/bgfx.h>
#include <string>


class Shader {
public:
    Shader(const std::string& vs_file, const std::string& fs_file);
    Shader(Shader&&) noexcept;
    ~Shader();

    // todo
    const auto& handle() { return program; }

private:
    bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;
};
