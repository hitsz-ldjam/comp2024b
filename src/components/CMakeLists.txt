add_library(components STATIC "")

# vvv add new sources file here vvv
target_sources(components
        PUBLIC
        transform.cpp
        camera.cpp
        )

target_include_directories(components
        PUBLIC ${THIRD_PARTY_INCLUDE_DIR}
        PUBLIC ${APP_SOURCE_DIR}
        )

target_link_libraries(components PUBLIC core)
