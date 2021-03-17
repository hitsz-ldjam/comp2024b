cmake_minimum_required(VERSION 3.15)
project(app)
set(CMAKE_CXX_STANDARD 17)

set(ROOT_DIR ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "bin/debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "bin/release")

set(THIRD_PARTY_BIN_DIR "${ROOT_DIR}/third_party/bins")
set(THIRD_PARTY_INCLUDE_DIR "${ROOT_DIR}/third_party/include")
set(THIRD_PARTY_LIB_DIR "${ROOT_DIR}/third_party/libs")
message(STATUS "third party include dir: ${THIRD_PARTY_INCLUDE_DIR}")
message(STATUS "third party lib dir: ${THIRD_PARTY_LIB_DIR}")

add_executable(main src/main.cpp)
target_sources(main
        PRIVATE
        src/app.cpp
        src/gui.cpp
        src/times.cpp
        src/input.cpp)

target_include_directories(main PRIVATE ${ROOT_DIR}/include)
set_property(TARGET main PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set_property(TARGET main PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${ROOT_DIR}")

# target_sources(main PRIVATE "{..}")
# add new sources file here ----^

# third party libs
target_include_directories(main PRIVATE ${THIRD_PARTY_INCLUDE_DIR})
# build system specific configurations
if (MSVC)
    target_include_directories(main
            PRIVATE
            ${THIRD_PARTY_INCLUDE_DIR}/_priv/compat/msvc)
elseif (MINGW)
    target_include_directories(main
            PRIVATE
            ${THIRD_PARTY_INCLUDE_DIR}/_priv/compat/mingw)
elseif (APPLE)
    target_include_directories(main
            PRIVATE
            ${THIRD_PARTY_INCLUDE_DIR}/_priv/compat/osx)
endif ()

set(THIRD_PARTY_LIB_NAMES
        SDL2
        SDL2main
        bgfx
        bimg
        bimg_decode
        bx
        imgui
        )

foreach (lib ${THIRD_PARTY_LIB_NAMES})
    target_link_libraries(main "${THIRD_PARTY_LIB_DIR}/$<IF:$<CONFIG:Debug>,debug,release>/${lib}.lib")
endforeach (lib ${THIRD_PARTY_LIB_NAMES})

# add third_party/bins to environment path or uncomment the following lines
# copy SDL2.dll to root directory
if (EXISTS ${ROOT_DIR}/SDL2.dll)
    message(STATUS "SDL2.dll found")
else ()
    message(STATUS "SDL2.dll not found, copy from ${THIRD_PARTY_BIN_DIR}/SDL2.dll")
    file(COPY ${THIRD_PARTY_BIN_DIR}/SDL2.dll DESTINATION ${ROOT_DIR})
endif ()
