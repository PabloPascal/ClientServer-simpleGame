cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(shooter2D)

find_package(SFML 2.5 COMPONENTS graphics window system audio network REQUIRED)

add_executable(shooter2D 
main.cpp
src/client.hpp
src/client.cpp
../../protocol.hpp
../../protocol.cpp

)

include_directories( 
src
)


target_link_libraries(${PROJECT_NAME}
    sfml-graphics
    sfml-window
    sfml-system
    sfml-network
    sfml-audio
)
