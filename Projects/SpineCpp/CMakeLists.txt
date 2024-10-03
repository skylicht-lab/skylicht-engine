cmake_minimum_required(VERSION 3.10)

include(flags.cmake)

include_directories(
	${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Include
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/System
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Engine
)

file(GLOB INCLUDES "spine-cpp/include/**/*.h")
file(GLOB SOURCES "spine-cpp/src/**/*.cpp")
file(GLOB SL_INCLUDES "*.h")
file(GLOB SL_SOURCES "*.cpp")

if (BUILD_SHARED_LIBS)
if (BUILD_LINUX OR BUILD_MACOS)
add_compile_options(-fpic)
endif()
endif()

add_library(SpineRuntimes STATIC ${SOURCES} ${INCLUDES} ${SL_SOURCES} ${SL_INCLUDES})

target_include_directories(SpineRuntimes PUBLIC spine-cpp/include)

target_link_libraries(SpineRuntimes Engine)