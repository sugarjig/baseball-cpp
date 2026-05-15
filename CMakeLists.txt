cmake_minimum_required(VERSION 4.2)
project(baseball_cpp)

set(CMAKE_CXX_STANDARD 26)

add_subdirectory(lib)

add_library(baseball_lib STATIC
        src/Simulator.cpp
        src/Game.cpp
        src/Scorebook.cpp
        src/GameState.cpp
        src/StaticEventSource.cpp
        include/Simulator.hpp
        include/Scorebook.hpp
        include/SimulatorObserver.hpp
        include/StaticEventSource.hpp
        include/EventSource.hpp
        include/Records.hpp
)
target_include_directories(baseball_lib PUBLIC include)
target_link_libraries(baseball_lib PUBLIC chadwick)

add_executable(baseball_cpp src/main.cpp)
target_link_libraries(baseball_cpp PRIVATE baseball_lib)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
)
# For Windows: prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

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
