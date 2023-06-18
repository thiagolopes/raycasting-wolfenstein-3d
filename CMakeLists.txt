# Project Definition
cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_PARALLEL_LEVEL 3)
set(BUILD_NAME game)
project(rayengine VERSION 0.3.0)

# Prerequisites
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

# Source Files
add_executable(${BUILD_NAME}
        src/imgui/imgui.cpp
        src/imgui/imgui_demo.cpp
        src/imgui/imgui_draw.cpp
        src/imgui/imgui_tables.cpp
        src/imgui/imgui_widgets.cpp
        src/imgui/backends/imgui_impl_sdl2.cpp
        src/imgui/backends/imgui_impl_opengl3.cpp
        src/main.cpp)


# Include Directories
target_include_directories(${BUILD_NAME} PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIR}
        ${GLUT_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/imgui
        ${CMAKE_SOURCE_DIR}/src/imgui/backends)

# Compiler Options
target_compile_options(${BUILD_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wunreachable-code
        -Wunused
        -Wignored-qualifiers
        -Wcast-align
        -Wformat-nonliteral
        -Wformat=2
        -Winvalid-pch
        -Wmissing-declarations
        -Wmissing-format-attribute
        -Wmissing-include-dirs
        -Wredundant-decls
        -Wswitch-default
        -Wodr
        -Wfatal-errors
        -fdiagnostics-color=always
        -pthread
        -pg
        -g)



# Link Libraries
target_link_libraries(${BUILD_NAME} Threads::Threads)
target_link_libraries(${BUILD_NAME} OpenGL::GL SDL2::SDL2 GLEW::GLEW m)


# file(GLOB SOURCES
#   "src/*.c"
#   "src/*.cpp"
# )
# add_executable(game ${SOURCES})

# Libs
# target_link_libraries(game OpenGL::GL SDL2::SDL2 GLEW::GLEW m SDL2_image)

#lint
add_custom_target(lint
    COMMAND clang-format --style=file src/*.cpp src/*.hpp  -i
)
