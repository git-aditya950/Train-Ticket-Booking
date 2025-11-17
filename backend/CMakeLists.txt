cmake_minimum_required(VERSION 3.15)
project(TrainTrackBackend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/lib/cpp-httplib
    ${CMAKE_SOURCE_DIR}/lib/nlohmann
)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Source files
file(GLOB_RECURSE SOURCES 
    "src/models/*.cpp"
    "src/controllers/*.cpp"
    "src/services/*.cpp"
    "src/utils/*.cpp"
)

# Add executable
add_executable(traintrack_server
    src/main.cpp
    ${SOURCES}
)

# Link libraries
target_link_libraries(traintrack_server
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(traintrack_server ws2_32)
endif()

# Copy any needed resources
# file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR})

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Source directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "Binary directory: ${CMAKE_BINARY_DIR}")
