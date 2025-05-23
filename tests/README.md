# Unit Testing for run1c

This directory contains unit tests for the run1c project using Google Test (gtest) framework.

## Test Files

- `test_utils.cpp` - Tests for utility functions
- `test_config.cpp` - Tests for configuration management
- `test_error_handler.cpp` - Tests for error handling functionality
- `test_main.cpp` - Main test runner

## Running Tests

To run the tests, build the project with CMake and then run the `run1c_tests` executable:

```bash
# From the project root directory
mkdir -p build
cd build
cmake ..
cmake --build .
ctest -V  # Run with Verbose output
```

Alternatively, you can run the test executable directly:

```bash
./run1c_tests
```

## Adding New Tests

To add new tests:

1. Create a new test file in the `tests` directory (e.g., `test_new_module.cpp`)
2. Add the file to the `TEST_SOURCES` list in `tests/CMakeLists.txt`
3. Write your tests using the Google Test framework

Example test:

```cpp
#include <gtest/gtest.h>
#include "my_module.h"

TEST(MyModuleTest, MyFunctionWorks) {
    EXPECT_EQ(myFunction(5), 25);
}
```

## Test Organization

Tests are organized by module, with each module having its own test file. Each test file contains one or more test fixtures and test cases.

## Code Coverage

To generate code coverage reports, build with coverage flags enabled:

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON ..
cmake --build .
ctest
# Generate coverage report (requires lcov)
lcov --directory . --capture --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```