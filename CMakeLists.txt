cmake_minimum_required(VERSION 3.14...3.31)
project(
    willengine
    LANGUAGES CXX C
)

## Enable decentralized package loading
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

## Install glfw
FetchContent_Declare(
    glfw3
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    )
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable( glfw3 )

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog/
    GIT_TAG v1.15.3
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    )
FetchContent_MakeAvailable( spdlog )

## Can change `dawn` to `wgpu`
set(WEBGPU_BACKEND "dawn" CACHE STRING "WebGPU backend (wgpu or dawn)")
FetchContent_Declare(
  webgpu
  GIT_REPOSITORY https://github.com/yig/WebGPU-distribution
  GIT_TAG        cs425-2025
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable( webgpu )

FetchContent_Declare(
  glfw3webgpu
  GIT_REPOSITORY https://github.com/eliemichel/glfw3webgpu
  GIT_TAG        fdcabcc54b56b50c12c10f5317abf8ae7ac32c29
  GIT_SHALLOW FALSE
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable( glfw3webgpu )

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 2d4c4b4dd31fde06cfffad7915c2b3006402322f
    GIT_SHALLOW FALSE
    GIT_PROGRESS TRUE
    )
FetchContent_MakeAvailable( glm )

## Declare the engine library
add_library( willengine STATIC src/Engine.cpp "src/Input/InputManager.cpp" "src/Graphics/GraphicsManager.cpp")
set_target_properties( willengine PROPERTIES CXX_STANDARD 20 )

## Declare our engine's header path
## This allows targets that depend on the engine to #include them
target_include_directories( willengine PUBLIC src )
target_link_libraries( willengine PUBLIC glfw )
target_link_libraries( willengine PRIVATE spdlog::spdlog )
target_link_libraries( willengine PUBLIC webgpu)
target_link_libraries( willengine PUBLIC glfw3webgpu)
target_link_libraries( willengine PUBLIC glm::glm)

add_executable( helloworld demo/helloworld.cpp)
set_target_properties( helloworld PROPERTIES CXX_STANDARD 20 )
target_link_libraries( helloworld PRIVATE willengine )
target_copy_webgpu_binaries( helloworld )
add_custom_target( run_helloworld helloworld USES_TERMINAL WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} )