# Testing Strategy for run1c

## Overview

This document outlines the testing approach for the run1c application. The project uses Google Test (gtest) as its testing framework to ensure reliability and correctness of the application's core functionality.

## Testing Structure

The tests are organized in the `tests/` directory with the following components:

- **Test Files**: Individual test files for each module
- **Test Fixtures**: Classes that set up and tear down test environments
- **Test Cases**: Individual test functions that verify specific behaviors

## Modules Under Test

### Utils Module (`test_utils.cpp`)

Tests for utility functions:
- `replaceWithRegex`: String replacement using regular expressions
- `replaceSubstring`: Simple string replacement
- `stringToWString`: String to wide string conversion
- `createFileIfNotExists`: File creation functionality

### Config Module (`test_config.cpp`)

Tests for configuration management:
- Font path handling
- 1C starter path configuration
- Font size settings
- Storage path settings
- Path validation

### Error Handler Module (`test_error_handler.cpp`)

Tests for error handling functionality:
- Path validation
- Error callback system
- Error message formatting
- Logging functionality

## Running Tests

### Command Line

To run tests from the command line:

```bash
# From the project root
.\run_tests.bat

# Or manually:
mkdir -p build
cd build
cmake ..
cmake --build .
ctest -V
```

### Visual Studio

1. Open the project in Visual Studio
2. Build the solution
3. Open Test Explorer
4. Run All Tests

## Adding New Tests

Follow these guidelines when adding new tests:

1. **Create Test File**: Add a new test file for new modules
2. **Update CMakeLists.txt**: Add the new test file to the `TEST_SOURCES` list
3. **Test Structure**:
   - For simple functions, use `TEST(TestSuite, TestName)`
   - For tests requiring setup/teardown, use fixtures with `TEST_F(FixtureName, TestName)`
4. **Meaningful Names**: Use descriptive names for test suites and test cases
5. **Assertions**: Use appropriate assertions for different types of checks:
   - `EXPECT_EQ` / `ASSERT_EQ` for equality checks
   - `EXPECT_TRUE` / `ASSERT_TRUE` for boolean checks
   - `EXPECT_THROW` / `ASSERT_THROW` for exception checks

## Test Coverage

Currently, test coverage includes:
- Core utility functions
- Configuration management
- Error handling

Future improvements to test coverage may include:
- UI component testing
- Integration tests for database launching
- Performance tests

## Mocking

For functions that depend on external systems (Windows API, file system), we use the following approaches:
1. Test with actual file system when feasible (creating temporary files)
2. Skip testing of complex system interactions (like process launching)

Future improvements could include creating mock objects for:
- File system operations
- Windows API calls
- Process creation

## Continuous Integration

Tests are designed to be run as part of a CI/CD pipeline. The `run_tests.bat` script provides a convenient way to build and run tests in automated environments.

## Best Practices

1. Keep tests independent of each other
2. Clean up any resources created during tests
3. Test both success and failure paths
4. Use descriptive error messages in assertions
5. Keep test code as simple as possible

## Troubleshooting

Common issues and solutions:

1. **Tests not found**: Make sure to run CMake configuration before building
2. **Build errors**: Check that all dependencies are properly linked
3. **Test failures**: Check test logs for details about specific failures
4. **Segmentation faults**: Look for memory management issues in tests