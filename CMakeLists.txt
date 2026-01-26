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
set(WEBGPU_BACKEND "wgpu" CACHE STRING "WebGPU backend (wgpu or dawn)")
FetchContent_Declare(
  webgpu
  GIT_REPOSITORY https://github.com/yig/WebGPU-distribution
  GIT_TAG        cs425-2025
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable( webgpu )

FetchContent_Declare(
  soloud
  GIT_REPOSITORY https://github.com/jarikomppa/soloud
  GIT_TAG        e82fd32c1f62183922f08c14c814a02b58db1873
  GIT_SHALLOW FALSE
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable( soloud )
## SoLoud doesn't have its own `CMakeLists.txt`, so let's declare a library directly from its sources.
file(GLOB soloud_sources "${soloud_SOURCE_DIR}/src/audiosource/*/*.c*" "${soloud_SOURCE_DIR}/src/c_api/*.c*" "${soloud_SOURCE_DIR}/src/core/*.c*" "${soloud_SOURCE_DIR}/src/filter/*.c*" "${soloud_SOURCE_DIR}/src/backend/miniaudio/soloud_miniaudio.cpp")
add_library(soloud ${soloud_sources})
target_compile_definitions(soloud PRIVATE WITH_MINIAUDIO)
target_include_directories(soloud PUBLIC "${soloud_SOURCE_DIR}/include")
if(APPLE)
    find_library(AudioUnit_LIBRARY AudioUnit)
    find_library(CoreAudio_LIBRARY CoreAudio)
    target_link_libraries(soloud INTERFACE ${AudioUnit_LIBRARY} ${CoreAudio_LIBRARY})
    target_compile_definitions(soloud PRIVATE MA_NO_RUNTIME_LINKING)
endif()
if(WIN32)
    target_compile_definitions( soloud PRIVATE MA_NO_JACK )
endif()

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

FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb/
  GIT_TAG        f58f558c120e9b32c217290b80bad1a0729fbb2c
  GIT_SHALLOW FALSE
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable( stb )

FetchContent_Declare(
    lua
    GIT_REPOSITORY https://github.com/walterschell/Lua
    GIT_TAG 504ef66d500fa1fb4f1684b6617b01342eee704a
    GIT_SHALLOW FALSE
    GIT_PROGRESS TRUE
    )
set(LUA_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable( lua )

FetchContent_Declare(
    sol2
    GIT_REPOSITORY https://github.com/yig/sol2
    GIT_TAG cs425-2025
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    )
FetchContent_MakeAvailable( sol2 )

# ============================================
# IMGUI
# ============================================
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.92.1
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(imgui)

## imgui doesn't have a `CMakeLists.txt`, so let's create a library for it.
file(GLOB imgui_sources 
    "${imgui_SOURCE_DIR}/*.c*" 
    "${imgui_SOURCE_DIR}/backends/imgui_impl_wgpu.cpp*" 
    "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp*"
)
add_library(imgui ${imgui_sources})
target_include_directories(imgui PUBLIC "${imgui_SOURCE_DIR}" "${imgui_SOURCE_DIR}/backends")
target_link_libraries(imgui PUBLIC glfw webgpu)
set_target_properties(imgui PROPERTIES CXX_STANDARD 17)

## Exactly one of IMGUI_IMPL_WEBGPU_BACKEND_DAWN or IMGUI_IMPL_WEBGPU_BACKEND_WGPU must be defined.
if(WEBGPU_BACKEND STREQUAL "dawn")
    target_compile_definitions(imgui PUBLIC IMGUI_IMPL_WEBGPU_BACKEND_DAWN)
elseif(WEBGPU_BACKEND STREQUAL "wgpu")
    target_compile_definitions(imgui PUBLIC IMGUI_IMPL_WEBGPU_BACKEND_WGPU)
else()
    message(FATAL_ERROR "Invalid WEBGPU_BACKEND value: ${WEBGPU_BACKEND}. Must be 'wgpu' or 'dawn'.")
endif()

## stb doesn't have a `CMakeLists.txt`. Let's declare a header-only library with an include path.
add_library( stb INTERFACE )
target_include_directories( stb INTERFACE ${stb_SOURCE_DIR} )

## Declare the engine library
add_library( willengine STATIC src/Engine.cpp "src/InputManager/InputManager.cpp" "src/GraphicsManager/GraphicsManager.cpp" "src/ResourceManager/ResourceManager.cpp" "src/ScriptManager/ScriptManager.cpp" "src/ECS/ECS.cpp" "src/SoundManager/SoundManager.cpp" "src/PhysicsManager/PhysicsManager.cpp" "src/SceneManager/SceneManager.cpp")
set_target_properties( willengine PROPERTIES CXX_STANDARD 20 )

## Declare our engine's header path
## This allows targets that depend on the engine to #include them
target_include_directories( willengine PUBLIC src )
target_link_libraries( willengine PUBLIC glfw )
target_link_libraries( willengine PRIVATE spdlog::spdlog )
target_link_libraries( willengine PUBLIC webgpu)
target_link_libraries( willengine PUBLIC glfw3webgpu)
target_link_libraries( willengine PUBLIC glfw spdlog::spdlog soloud )
target_link_libraries( willengine PUBLIC glm::glm)
target_link_libraries( willengine PUBLIC stb)
target_link_libraries( willengine PUBLIC lua_static)
target_link_libraries( willengine PUBLIC sol2)

add_executable( helloworld demo/helloworld.cpp)
set_target_properties( helloworld PROPERTIES CXX_STANDARD 20 )
target_link_libraries( helloworld PRIVATE willengine )
target_copy_webgpu_binaries( helloworld )
add_custom_target( run_helloworld helloworld USES_TERMINAL WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} )

# ============================================
# EDITOR
# ============================================
add_executable(editor Editor/editor.cpp "Editor/UserInterface/UI.cpp" "Editor/App.cpp" "Editor/EventHandler/EventHandler.cpp")
set_target_properties(editor PROPERTIES CXX_STANDARD 20)
target_link_libraries(editor PRIVATE willengine imgui)
target_compile_definitions(editor PRIVATE WILLENGINE_EDITOR=1)
target_copy_webgpu_binaries(editor)
add_custom_target(run_editor editor USES_TERMINAL WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})