cmake_minimum_required(VERSION 3.10)
project(EMSim VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

add_executable(main src/main.cpp src/Simulation.cpp)


target_link_libraries(main sfml-graphics)

target_include_directories(main PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/metal-cpp"
)

target_link_libraries(main
    "-framework Metal"
    "-framework QuartzCore"
    "-framework Foundation"
)

# add_custom_target(metal_shaders DEPENDS shaders.metallib)
# add_dependencies(main metal_shaders)
