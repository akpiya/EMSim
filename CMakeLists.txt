cmake_minimum_required(VERSION 3.10)
project(EMSim VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

add_executable(main src/main.cpp src/Simulation.cpp)

# add_custom_command(
#     OUTPUT shaders.air
#     COMMAND xcrun -sdk macosx metal -c ${CMAKE_SOURCE_DIR}/src/Compute.metal -o ${CMAKE_BINARY_DIR}/shaders.air
#     MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/src/Compute.metal
#     COMMENT "COMPILE METAL SHADER"
# )
#
# add_custom_command(
#     OUTPUT shaders.metallib
#     COMMAND xcrun -sdk macosx metallib ${CMAKE_BINARY_DIR}/shaders.air -o ${CMAKE_BINARY_DIR}/shaders.metallib
#     DEPENDS shaders.air
#     COMMENT "Creating the library"
# )
#
# find_library(METAL Metal REQUIRED)
# find_library(FOUNDATION Foundation REQUIRED)
# find_library(QUARTZCORE QuartzCore REQUIRED)
#
# target_link_libraries(main
#     ${METAL}
#     ${FOUNDATION}
#     ${QUARTZCORE}
#     sfml-graphics
# )
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

