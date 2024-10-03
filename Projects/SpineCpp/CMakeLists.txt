cmake_minimum_required(VERSION 3.10)

include(flags.cmake)

include_directories(include)
file(GLOB INCLUDES "spine-cpp/include/**/*.h")
file(GLOB SOURCES "spine-cpp/src/**/*.cpp")

if (BUILD_SHARED_LIBS)
if (BUILD_LINUX OR BUILD_MACOS)
add_compile_options(-fpic)
endif()
endif()

add_library(spine-cpp STATIC ${SOURCES} ${INCLUDES})
target_include_directories(spine-cpp PUBLIC spine-cpp/include)