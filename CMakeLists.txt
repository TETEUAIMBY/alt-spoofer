cmake_minimum_required(VERSION 3.16)
project(AltSpoofer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

# Create executable
add_executable(AltSpoofer ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(AltSpoofer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Link libraries for Windows
if(WIN32)
    target_link_libraries(AltSpoofer PRIVATE ws2_32 wbemuuid oleaut32 ole32)
endif()