# Project Definition
cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_STANDARD 99)
set(BUILD_NAME rayengine)
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
        src/main.c)


# Include Directories
target_include_directories(${BUILD_NAME} PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIR}
        ${GLUT_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/src)

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

#lint
add_custom_target(lint
    COMMAND clang-format --style=file src/*.c src/*.h  -i
 src/*.h  -i
)
