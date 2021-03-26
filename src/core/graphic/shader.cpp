#include "shader.h"
#include "binary_reader.h"

#include <bgfx/bgfx.h>

#include <cassert>


Shader::Shader(const std::string& vs_file, const std::string& fs_file) {
    assert(bgfx::getRendererType() == bgfx::RendererType::Direct3D11);
    auto alloc = [](auto size) { return bgfx::alloc(size); };
    auto get = [](auto mem) { return mem->data; };
    auto vs_data = read_binary(vs_file, alloc, get);
    auto vs = bgfx::createShader(vs_data);
    auto fs_data = read_binary(fs_file, alloc, get);
    auto fs = bgfx::createShader(fs_data);

    program = bgfx::createProgram(vs, fs, true);
}


Shader::Shader(Shader&& other) noexcept : program(other.program) {
    other.program = BGFX_INVALID_HANDLE;
}


Shader::~Shader() {
    if(bgfx::isValid(program))
        bgfx::destroy(program);
    program = BGFX_INVALID_HANDLE;
}
