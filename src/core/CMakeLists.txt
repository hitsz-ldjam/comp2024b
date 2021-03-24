add_library(core STATIC "")

# vvv add new sources file here vvv
target_sources(core
        PUBLIC
        app.cpp
        gui.cpp
        times.cpp
        input.cpp
        gfx.cpp
        window.cpp
        screen.cpp
        model.cpp
        )

target_include_directories(core
        PUBLIC ${THIRD_PARTY_INCLUDE_DIR}
        PUBLIC ${APP_SOURCE_DIR}
        )

target_link_libraries(core PUBLIC $<IF:$<CONFIG:DEBUG>,${THIRD_PARTY_LIBS_DEBUG},${THIRD_PARTY_LIBS_RELEASE}>)