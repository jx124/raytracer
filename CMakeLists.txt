cmake_minimum_required(VERSION 3.15)
project(raytracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS)

add_compile_options(-Wall -Wextra)

if(CMAKE_BUILD_TYPE STREQUAL "debug")
    add_compile_definitions(DEBUG=1)
    add_compile_options(-fsanitize=address -fsanitize=leak -fsanitize=undefined -fstack-protector-strong -pipe)
    add_link_options(-fsanitize=address -fsanitize=undefined)
else(CMAKE_BUILD_TYPE STREQUAL "release")
    add_compile_options(-O2 -march=native -flto -pipe)
endif()

file(GLOB_RECURSE source CONFIGURE_DEPENDS 
    "src/*.cpp"
    "src/glad.c"
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("include/glfw")

add_executable(raytracer ${source})

target_include_directories(raytracer PUBLIC "include/")
target_link_libraries(raytracer PUBLIC dl glfw)