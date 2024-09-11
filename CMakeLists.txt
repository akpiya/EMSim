cmake_minimum_required(VERSION 3.10)
project(EMSim VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)


add_executable(main src/main.cpp src/Simulation.cpp)
add_executable(debug_main src/profile_main.cpp src/Simulation.cpp src/Profiler.cpp)

target_link_libraries(main PRIVATE sfml-graphics)
target_include_directories(main PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(debug_main PRIVATE sfml-graphics)
target_include_directories(debug_main PUBLIC ${PROJECT_SOURCE_DIR}/include)


