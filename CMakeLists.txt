cmake_minimum_required(VERSION 3.5)
project(rayengine)

file(GLOB SOURCES
  "src/*.c"
)
add_executable(exec ${SOURCES})

# Libs
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)

target_link_libraries(exec OpenGL::GL SDL2::SDL2 GLEW::GLEW m SDL2_image)

#lint
add_custom_target(lint
    COMMAND clang-format --style=file src/*.c -i
)
