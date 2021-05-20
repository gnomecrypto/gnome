cmake_minimum_required(VERSION 3.8)

project(gnome_app VERSION 0.1)

set(
    SOURCES
    main.cpp
)

include_directories(${gnome_core_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} gnome_core)