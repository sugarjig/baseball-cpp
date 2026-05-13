cmake_minimum_required(VERSION 4.2)
project(baseball_cpp)

set(CMAKE_CXX_STANDARD 26)

add_subdirectory(vendor)

add_executable(baseball_cpp main.cpp
        Simulator.cpp
        Simulator.h
        SimulatorObserver.h
        StaticEventSource.cpp
        StaticEventSource.h
        EventSource.h
)
target_link_libraries(baseball_cpp PRIVATE chadwick)
