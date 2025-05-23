@echo off
setlocal enabledelayedexpansion

echo Building and Running Tests for run1c
echo -----------------------------------

:: Create build directory if it doesn't exist
if not exist build mkdir build

:: Navigate to build directory
cd build

:: Configure with CMake
echo Configuring with CMake...
cmake .. || (
    echo CMake configuration failed!
    exit /b 1
)

:: Build the project
echo Building project...
cmake --build . || (
    echo Build failed!
    exit /b 1
)

:: Run the tests
echo Running tests...
ctest -V || (
    echo Some tests failed!
    exit /b 1
)

echo All tests passed successfully!
exit /b 0