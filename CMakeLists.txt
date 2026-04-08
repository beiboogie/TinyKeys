cmake_minimum_required(VERSION 3.29)
project(TinyKeys C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

add_executable(tinykeys
    src/main.c
    src/data_config.c
    src/audio_engine.c
    src/tui_renderer.c
)

if(WIN32)
    target_link_libraries(tinykeys winmm)
endif()
