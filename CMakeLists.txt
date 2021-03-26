cmake_minimum_required(VERSION 3.15)
project(app)
set(CMAKE_CXX_STANDARD 17)

set(APP_ROOT_DIR ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "bin/debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "bin/release")

# third party libs
set(THIRD_PARTY_BIN_DIR "${APP_ROOT_DIR}/third_party/bins")
set(THIRD_PARTY_LIB_DIR "${APP_ROOT_DIR}/third_party/libs")
set(THIRD_PARTY_INCLUDE_DIR "${APP_ROOT_DIR}/third_party/include")
if (MSVC)
    list(APPEND THIRD_PARTY_INCLUDE_DIR ${THIRD_PARTY_INCLUDE_DIR}/_priv/compat/msvc)
elseif (MINGW)
    list(APPEND THIRD_PARTY_INCLUDE_DIR ${THIRD_PARTY_INCLUDE_DIR}/_priv/compat/mingw)
elseif (APPLE)
    list(APPEND THIRD_PARTY_INCLUDE_DIR ${THIRD_PARTY_INCLUDE_DIR}/_priv/compat/osx)
endif ()

set(THIRD_PARTY_LIBS_DEBUG "")
set(THIRD_PARTY_LIBS_RELEASE "")
set(THIRD_PARTY_LIB_NAMES
        SDL2
        SDL2main
        bgfx
        bimg
        bimg_decode
        bx
        imgui
        tiny_obj_loader
        OpenFBX
        )
foreach (lib ${THIRD_PARTY_LIB_NAMES})
    list(APPEND THIRD_PARTY_LIBS_DEBUG "${THIRD_PARTY_LIB_DIR}/debug/${lib}.lib")
    list(APPEND THIRD_PARTY_LIBS_RELEASE "${THIRD_PARTY_LIB_DIR}/release/${lib}.lib")
endforeach (lib ${THIRD_PARTY_LIB_NAMES})

message(STATUS "third party include dir: ${THIRD_PARTY_INCLUDE_DIR}")
message(STATUS "third party lib dir: ${THIRD_PARTY_LIB_DIR}")

set(APP_SOURCE_DIR ${APP_ROOT_DIR}/src)
add_executable(main src/main.cpp)
target_include_directories(main
        PUBLIC ${APP_SOURCE_DIR}
        PRIVATE ${THIRD_PARTY_INCLUDE_DIR}
        )
set_property(TARGET main PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set_property(TARGET main PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${APP_ROOT_DIR}")

add_subdirectory(src/core)
add_subdirectory(src/components)
# add_subdirectory(src/systems)

target_link_libraries(main core)
target_link_libraries(main components)
# target_link_libraries(main systems)

# add third_party/bins to environment path or uncomment the following lines
# copy SDL2.dll to root directory
if (EXISTS ${APP_ROOT_DIR}/SDL2.dll)
    message(STATUS "SDL2.dll found")
else ()
    message(STATUS "SDL2.dll not found, copy from ${THIRD_PARTY_BIN_DIR}/SDL2.dll")
    file(COPY ${THIRD_PARTY_BIN_DIR}/SDL2.dll DESTINATION ${APP_ROOT_DIR})
endif ()
