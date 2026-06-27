cmake_minimum_required(VERSION 3.15)
project(Yuuki VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Add source files
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")

# Create executable
add_executable(yuuki ${SOURCES})

# Include directories
target_include_directories(yuuki PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Platform-specific optimizations
if(MSVC)
    target_compile_options(yuuki PRIVATE /O2 /arch:AVX2)
elseif(APPLE)
    # Apple Silicon or Intel
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
        target_compile_options(yuuki PRIVATE -O3 -mcpu=apple-m1)
    else()
        target_compile_options(yuuki PRIVATE -O3 -march=native)
    endif()
else()
    # Linux and other Unix-like systems
    target_compile_options(yuuki PRIVATE -O3 -march=native -flto)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_link_options(yuuki PRIVATE -flto)
    endif()
endif()

# Enable threading support
find_package(Threads REQUIRED)
target_link_libraries(yuuki PRIVATE Threads::Threads)

# Installation
install(TARGETS yuuki DESTINATION bin)
