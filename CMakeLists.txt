cmake_minimum_required(VERSION 4.2)
project(baseball_cpp)

set(CMAKE_CXX_STANDARD 26)

add_subdirectory(lib)

add_executable(baseball_cpp
        src/main.cpp
        src/Simulator.cpp
        src/Game.cpp
        src/StaticEventSource.cpp
        include/Simulator.hpp
        include/SimulatorObserver.hpp
        include/StaticEventSource.hpp
        include/EventSource.hpp
)
target_include_directories(baseball_cpp PRIVATE include)
target_link_libraries(baseball_cpp PRIVATE chadwick)
