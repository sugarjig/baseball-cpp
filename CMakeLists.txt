cmake_minimum_required(VERSION 4.2)
project(baseball_cpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 26)

# Add local vcpkg_installed directory to search path if it exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed")
    file(GLOB VCPKG_INSTALLED_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/*")
    foreach(DIR ${VCPKG_INSTALLED_DIRS})
        if(IS_DIRECTORY "${DIR}")
            list(APPEND CMAKE_PREFIX_PATH "${DIR}")
        endif()
    endforeach()
endif()

add_subdirectory(lib)

add_library(baseball_lib STATIC
        src/Simulator.cpp
        src/chadwick/Game.cpp
        src/chadwick/Scorebook.cpp
        src/chadwick/GameState.cpp
        src/StaticEventSource.cpp
        src/RandomEventSource.cpp
        src/MatrixEventSource.cpp
        src/MatrixLoader.cpp
        include/Simulator.hpp
        include/chadwick/Scorebook.hpp
        include/chadwick/Game.hpp
        include/chadwick/GameState.hpp
        include/SimulatorObserver.hpp
        include/StaticEventSource.hpp
        include/RandomEventSource.hpp
        include/MatrixEventSource.hpp
        include/MatrixLoader.hpp
        include/MatrixData.hpp
        include/EventSource.hpp
        include/Records.hpp
        include/IGame.hpp
        include/IGameState.hpp
)
target_include_directories(baseball_lib PUBLIC include)
target_link_libraries(baseball_lib PUBLIC chadwick)

add_executable(baseball_cpp src/main.cpp)
target_link_libraries(baseball_cpp PRIVATE baseball_lib)

find_package(GTest CONFIG REQUIRED)

add_executable(normalize_retrosheet tools/normalize_retrosheet.cpp)
target_link_libraries(normalize_retrosheet PRIVATE baseball_lib)

enable_testing()

add_subdirectory(tests)
