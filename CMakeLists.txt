cmake_minimum_required(VERSION 3.16)

project(YINI-pp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Header-only library target
add_library(yini-pp INTERFACE)

# Specify the include directory for the library
target_include_directories(yini-pp INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Enable testing
enable_testing()

# Add tests subdirectory
add_subdirectory(tests)

