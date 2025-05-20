cmake_minimum_required(VERSION 3.30)
project(run1c)

# Project settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall")
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

# Version and Platform
set(EXECUTABLE_VERSION "1.0.0")
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" PLATFORM)

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Dependencies

set(FT_DISABLE_ZLIB ON)
set(FT_DISABLE_BZIP2 ON)
set(FT_DISABLE_PNG ON)
set(FT_DISABLE_HARFBUZZ ON)
set(FT_DISABLE_BROTLI ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/freetype-2.13.3/)

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
    src/main.cpp
)

set(project_include_dir
    src
)

set(project_headers
    ${project_include_dir}/my_imgui_config.h
    ${project_include_dir}/utils.h
)

add_executable(run1c ${project_headers} ${project_srcs} ${imgui_srcs})
set_target_properties(run1c PROPERTIES OUTPUT_NAME "run1c_v${EXECUTABLE_VERSION}_${PLATFORM}")
target_compile_options(run1c PRIVATE -g) # Add debug symbols
target_include_directories(run1c PRIVATE src)
target_link_libraries(run1c SDL2main SDL2 opengl32 freetype)
