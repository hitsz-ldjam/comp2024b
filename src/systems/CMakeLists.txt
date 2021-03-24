add_library(systems STATIC "")

# vvv add new sources file here vvv
target_sources(systems
        ""
        )

target_include_directories(systems
        PUBLIC ${THIRD_PARTY_INCLUDE_DIR}
        PUBLIC ${APP_SOURCE_DIR}
        )

target_link_libraries(systems PUBLIC core)
