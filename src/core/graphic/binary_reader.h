#pragma once

#include <functional>
#include <fstream>
#include <string>


/// @param alloc A function that takes an integer size and returns a buffer
/// @param get A function that takes a (suggested to be const) reference to the above buffer and return a pointer to its content
template<typename A, typename G>
auto read_binary(const std::string& filename, A alloc, G get) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    const auto filesize = file.tellg();
    file.seekg(0, std::ios::beg);
    auto buffer = alloc(filesize);
    file.read((char*)get(buffer), filesize);
    file.close();
    //return std::make_pair(buffer, filesize);
    return buffer;
}


/// @tparam T buffer type
/// @see read_binary
template<typename T>
auto read_binary(const std::string& filename, const std::function<T(size_t)>& alloc, const std::function<void*(T&)>& get) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    const auto filesize = file.tellg();
    file.seekg(0, std::ios::beg);
    auto buffer = alloc(filesize);
    file.read((char*)get(buffer), filesize);
    file.close();
    return buffer;
}
