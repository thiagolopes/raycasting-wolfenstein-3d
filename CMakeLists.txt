cmake_minimum_required(VERSION 3.5)
project(rayengine)

file(GLOB SOURCES
  "src/*.c"
  "src/*.cpp"
)
add_executable(game ${SOURCES})

# Libs
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

target_link_libraries(game OpenGL::GL SDL2::SDL2 GLEW::GLEW m SDL2_image)

#lint
add_custom_target(lint
    COMMAND clang-format --style=file src/*.cpp -i
)
