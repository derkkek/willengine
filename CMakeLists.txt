cmake_minimum_required(VERSION 3.14...3.31)
project(
    willengine
    LANGUAGES CXX C
)
## Declare the engine library
add_library( willengine STATIC src/Engine.cpp )
set_target_properties( willengine PROPERTIES CXX_STANDARD 20 )

## Declare our engine's header path
## This allows targets that depend on the engine to #include them
target_include_directories( willengine PUBLIC src )

add_executable( helloworld demo/helloworld.cpp )
set_target_properties( helloworld PROPERTIES CXX_STANDARD 20 )
target_link_libraries( helloworld PRIVATE willengine )