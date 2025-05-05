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
    main.cpp
)

# Debug executable
add_executable(run1c_debug ${project_srcs} ${imgui_srcs})
set_target_properties(run1c_debug PROPERTIES OUTPUT_NAME "run1c_debug_v${EXECUTABLE_VERSION}_${PLATFORM}")
target_compile_options(run1c_debug PRIVATE -g) # Add debug symbols
target_link_libraries(run1c_debug SDL2main SDL2 opengl32 freetype)
# # Production executable
# add_executable(run1c_production WIN32 ${project_srcs} ${imgui_srcs})
# set_target_properties(run1c_production PROPERTIES OUTPUT_NAME "run1c_production_v${EXECUTABLE_VERSION}_${PLATFORM}")
# # target_compile_options(run1c_production PRIVATE -O2) # Add optimization
# # target_compile_definitions(run1c_production PRIVATE NDEBUG) # Disable asserts
# target_link_libraries(run1c_production SDL2main.lib SDL2.lib opengl32.lib)
