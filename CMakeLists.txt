cmake_minimum_required(VERSION 3.20)
project(run1c)

# Project settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# Enable testing
enable_testing()

# Version and Platform
set(EXECUTABLE_VERSION "1.0.0")
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" PLATFORM)

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Include FetchContent for managing dependencies
include(FetchContent)

# Setup Google Test
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

find_package(Freetype REQUIRED)

if(FREETYPE_FOUND)
    message(STATUS "Found freetype library: ${FREETYPE_INCLUDE_DIRS}")
else()
    message(STATUS "freetype library not found")
endif()

include_directories(${FREETYPE_INCLUDE_DIRS})

# Dependencies

# set(FT_DISABLE_ZLIB ON)
# set(FT_DISABLE_BZIP2 ON)
# set(FT_DISABLE_PNG ON)
# set(FT_DISABLE_HARFBUZZ ON)
# set(FT_DISABLE_BROTLI ON)

# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/freetype-2.13.3/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL2-2.32.4/include/)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL2-2.32.4/lib/x64/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui-1.91.9b/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui-1.91.9b/misc/cpp/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui-1.91.9b/backends/)

# Source files
set(imgui_srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui-1.91.9b/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui-1.91.9b/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui-1.91.9b/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui-1.91.9b/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui-1.91.9b/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui-1.91.9b/misc/cpp/imgui_stdlib.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui-1.91.9b/backends/imgui_impl_sdl2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui-1.91.9b/backends/imgui_impl_opengl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui-1.91.9b/misc/freetype/imgui_freetype.h
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui-1.91.9b/misc/freetype/imgui_freetype.cpp
)

set(project_srcs
    src/utils.cpp
    src/config.cpp
    src/error_handler.cpp
)

set(project_include_dir
    src
)

set(project_headers
    ${project_include_dir}/my_imgui_config.h
    ${project_include_dir}/utils.h
    ${project_include_dir}/config.h
    ${project_include_dir}/error_handler.h
)

# Create a static library for the core code (to be used in tests)
add_library(run1c_lib STATIC ${project_headers} ${project_srcs})
target_include_directories(run1c_lib PUBLIC ${project_include_dir})
target_link_libraries(run1c_lib freetype ${CMAKE_DL_LIBS})

# Create the main executable
add_executable(run1c src/main.cpp ${imgui_srcs})
set_target_properties(run1c PROPERTIES OUTPUT_NAME "run1c_v${EXECUTABLE_VERSION}_${PLATFORM}")
target_compile_options(run1c PRIVATE -g) # Add debug symbols
target_include_directories(run1c PRIVATE src)
target_link_libraries(run1c run1c_lib SDL2main SDL2 opengl32 freetype)

# Add tests subdirectory
add_subdirectory(tests)
