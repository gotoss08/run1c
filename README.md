# RUN1C - 1C:Enterprise Database Launcher

A modern GUI application for launching 1C:Enterprise databases with an intuitive interface and persistent history.

## Features

- **Smart Path Detection**: Automatically extracts database paths from various input formats
- **History Management**: Persistent storage of previously used database paths
- **Dual Launch Modes**:
  - Enterprise mode (Enter)
  - Configuration mode (Shift+Enter)
- **DPI Aware**: Automatic scaling for high-DPI displays
- **Cyrillic Support**: Full Unicode support with proper font rendering
- **Keyboard Shortcuts**: Fast navigation with F key and arrow keys

## System Requirements

- Windows 10/11 (x64)
- 1C:Enterprise installed
- OpenGL 3.0+ compatible graphics
- Visual C++ Redistributable

## Building from Source

### Prerequisites

- CMake 3.20+
- Visual Studio 2019+ or equivalent C++20 compiler
- Git

### Dependencies (Included)

- SDL2 2.32.4
- Dear ImGui 1.91.9b
- FreeType 2.13.3
- Google Test (fetched automatically for testing)

### Build Instructions

```bash
git clone git@github.com:gotoss08/run1c.git
cd run1c
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

Executable will be generated in `bin/` directory.

### Running Tests

```bash
cd build
ctest -V  # Run tests with verbose output
```

Or run the test executable directly:

```bash
./run1c_tests
```

## Configuration

The application automatically detects your 1C installation. For custom configurations:

- **1C Path**: Auto-detected from `%PROGRAMFILES%\1cv8\common\1cestart.exe`
- **Font**: Uses system Segoe UI font with FreeType rendering
- **Storage**: History saved to `run1c_storage.ini`

## Usage

1. Launch the application
2. Enter database path in the search field
3. Press Enter for Enterprise mode or Shift+Enter for Configuration mode
4. Use F key to focus the search field
5. Navigate history with up/down arrows

### Supported Path Formats

- Full file paths: `C:\Databases\MyBase\1Cv8.1cd` (directly to 1Cv8.1cd file, any case: 1CV8.1CD, 1cv8.1cd, etc.)
- Directory paths: `C:\Databases\MyBase` (containing 1Cv8.1cd file)
- UNC paths: `\\server\share\database\1Cv8.1cd`
- Quoted paths: `"C:\Path with spaces\database.1cd"`

### Recent Improvements

### Path Handling Enhancements
- **1Cv8.1cd Path Support**: Added support for directly specifying the path to a 1Cv8.1cd file (case-insensitive)
- **Automatic Path Resolution**: Automatically locates the parent directory when pointing to 1Cv8.1cd files
- **Improved Path Validation**: Enhanced path extraction and validation with case-insensitive filename comparison

### Testing Infrastructure
- **Unit Testing**: Added comprehensive tests with Google Test framework
- **Test Coverage**: Tests for utility functions, configuration and error handling
- **Test Automation**: Integrated with CTest for automated test execution

### Architecture Enhancements

- **Separated Headers and Implementation**: Fixed ODR violations by moving function definitions from headers to .cpp files
- **Smart Pointer Usage**: Replaced raw pointers with `std::unique_ptr` for better memory management
- **Configuration System**: Centralized configuration management with `Config` class
- **Error Handling**: Comprehensive error handling with `ErrorHandler` class
- **Path Validation**: Robust file system validation before process execution

### Code Quality

- **Modern C++20**: Leveraging latest language features
- **Exception Safety**: Proper exception handling throughout the application
- **Logging System**: Centralized logging with file output
- **Input Validation**: Comprehensive validation of user inputs and file paths

### Security Improvements

- **Process Execution**: Secure process launching with proper argument escaping
- **Path Validation**: File system validation before any operations
- **Error Boundaries**: Contained error handling preventing crashes

### Build System

- **CMake 3.20**: Updated build system requirements (NOTE: CMakeLists.txt may need to be updated)
- **Modular Structure**: Clean separation of components
- **Dependency Management**: Proper inclusion of all source files

## File Structure

```
src/
├── main.cpp              # Application entry point and UI
├── config.h/.cpp         # Configuration management
├── error_handler.h/.cpp  # Error handling and validation
├── utils.h/.cpp          # Utility functions
├── my_imgui_config.h     # ImGui configuration
tests/
├── test_utils.cpp        # Tests for utility functions
├── test_config.cpp       # Tests for configuration
├── test_error_handler.cpp # Tests for error handler
└── test_main.cpp         # Test entry point
vendor/
├── SDL2-2.32.4/          # Windowing and input
├── imgui-1.91.9b/        # Immediate mode GUI
└── freetype-2.13.3/      # Font rendering
```

## Keyboard Shortcuts

- `F` - Focus search input
- `Enter` - Launch in Enterprise mode
- `Shift+Enter` - Launch in Configuration mode
- `Up/Down Arrow` - Navigate history
- `Escape` - Exit application

## Error Handling

The application provides comprehensive error reporting:

- Path validation before launching
- 1C installation detection
- Font loading verification
- Process execution monitoring
- Detailed error logging to `run1c_error.log`

## Future Improvements

- Cross-platform support
- Advanced UI features (settings dialog, themes)
- Configuration file system
- Enhanced path handling
- Internationalization
- Performance optimizations

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

Copyright (c) 2025 Pavel Pereverzev (gotoss08@gmail.com)

## Changelog

### Version 1.1.0
- Added support for launching bases using full path to 1Cv8.1cd file (case-insensitive)
- Improved path extraction and validation
- Enhanced error logging for path validation

### Version 1.0.0
- Initial release with modern C++ architecture
- Fixed ODR violations and memory management
- Added comprehensive error handling
- Implemented configuration system
- Enhanced security and validation
- Improved build system
