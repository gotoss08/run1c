# Tests CMakeLists.txt

# Enable testing
enable_testing()

# Add test files
set(TEST_SOURCES
    test_utils.cpp
    test_config.cpp
    test_error_handler.cpp
    test_main.cpp
)

# Create test executable
add_executable(run1c_tests ${TEST_SOURCES})

# Link with Google Test and project library
target_link_libraries(run1c_tests
    gtest
    gtest_main
    run1c_lib
)

# Link with other dependencies required by the project
target_link_libraries(run1c_tests
    SDL2
    freetype
)

target_include_directories(run1c_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
)

# Register tests
include(GoogleTest)
gtest_discover_tests(run1c_tests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})