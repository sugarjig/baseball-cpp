cmake_minimum_required(VERSION 3.31.6)
project(baseball_cpp)

set(CMAKE_CXX_STANDARD 26)

add_subdirectory(lib)

add_library(baseball_lib STATIC
        src/Simulator.cpp
        src/chadwick/Game.cpp
        src/chadwick/Scorebook.cpp
        src/chadwick/GameState.cpp
        src/StaticEventSource.cpp
        include/Simulator.hpp
        include/chadwick/Scorebook.hpp
        include/chadwick/Game.hpp
        include/chadwick/GameState.hpp
        include/SimulatorObserver.hpp
        include/StaticEventSource.hpp
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

add_executable(baseball_tests
        tests/SimulatorTests.cpp
        tests/SimulatorIntegrationTests.cpp
)
target_compile_definitions(baseball_tests PRIVATE PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(baseball_tests
    PRIVATE
    baseball_lib
    GTest::gtest_main
    GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(baseball_tests)
