file(GLOB_RECURSE UNIT_TEST_SOURCES CONFIGURE_DEPENDS "unit/*.cpp")
file(GLOB_RECURSE INTEGRATION_TEST_SOURCES CONFIGURE_DEPENDS "integration/*.cpp")

# Unit Tests
add_executable(baseball_unit_tests ${UNIT_TEST_SOURCES})
target_compile_definitions(baseball_unit_tests PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
target_link_libraries(baseball_unit_tests
    PRIVATE
    baseball_lib
    GTest::gtest_main
    GTest::gmock
)

# Integration Tests
add_executable(baseball_integration_tests ${INTEGRATION_TEST_SOURCES})
target_compile_definitions(baseball_integration_tests PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
target_link_libraries(baseball_integration_tests
    PRIVATE
    baseball_lib
    GTest::gtest_main
    GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(baseball_unit_tests PROPERTIES LABELS "unit")
gtest_discover_tests(baseball_integration_tests PROPERTIES LABELS "integration")
