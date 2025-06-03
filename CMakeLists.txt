cmake_minimum_required(VERSION 3.10)
project(DJIOsdkCommunicationApp CXX)

set(CMAKE_CXX_STANDARD 17) # Using C++17 for std::filesystem
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the output directory for the executable to be in the build directory
# This is the default CMake behavior and usually fine.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


# Find DJI OSDK package
find_package(DJIOSDK REQUIRED)

# Add include directories
# This allows #include "dji-osdk-communication-app/video_controller.hpp"
target_include_directories(${PROJECT_NAME} PUBLIC
    "${PROJECT_SOURCE_DIR}/include" # For your project's headers
    ${DJIOSDK_INCLUDE_DIRS}         # For OSDK headers
)


# Add the executable and its source files
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/video_controller.cpp  # Add the new source file
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    DJIOSDK::dji_osdk_core
    pthread
)

# On some systems, especially for std::filesystem with older libstdc++, we might need:
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(${PROJECT_NAME} PRIVATEstdc++fs) # For GCC < 9
endif()