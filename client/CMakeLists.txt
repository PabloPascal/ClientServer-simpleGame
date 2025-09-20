cmake_minimum_required(VERSION 3.10.0)
project(Client VERSION 0.1.0 LANGUAGES C CXX)

find_package(SFML 2.5 COMPONENTS graphics window system audio network REQUIRED)

add_executable(Client
client.cpp
)

target_link_libraries(${PROJECT_NAME}
    sfml-graphics
    sfml-window
    sfml-system
    sfml-network
    sfml-audio
)
