cmake_minimum_required(VERSION 3.10)

include(flags.cmake)

include_directories(
	${SKYLICHT_ENGINE_PROJECT_DIR}/SpineCpp/spine-cpp/include
	${SKYLICHT_ENGINE_PROJECT_DIR}/SpineCpp
	${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Include
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/System
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Engine
)

file(GLOB SP_INCLUDES "spine-cpp/include/**/*.h")
file(GLOB SP_SOURCES "spine-cpp/src/**/*.cpp")
file(GLOB SL_INCLUDES "spine-runtimes/*.h")
file(GLOB SL_SOURCES "spine-runtimes/*.cpp")

add_library(SpineRuntimes ${ENGINE_SHARED_OR_STATIC_LIB} ${SP_SOURCES} ${SP_INCLUDES} ${SL_SOURCES} ${SL_INCLUDES})

if (BUILD_SHARED_LIBS)
if (BUILD_LINUX OR BUILD_MACOS)
add_compile_options(-fpic)
endif()

set_target_properties(SpineRuntimes PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
add_definitions(-DSPINE_EXPORTS)
endif()

target_link_libraries(SpineRuntimes Engine)

if (INSTALL_LIBS)
install(TARGETS SpineRuntimes
			EXPORT SpineRuntimesTargets
			RUNTIME DESTINATION ${SKYLICHT_RUNTIME_INSTALL_DIR}
			LIBRARY DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			ARCHIVE DESTINATION ${SKYLICHT_ARCHIVE_INSTALL_DIR})

install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/SpineCpp/spine-runtimes
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/SpineCpp
	FILES_MATCHING PATTERN "*.h*")

install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/SpineCpp/spine-cpp/include/spine
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/SpineCpp
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT SpineRuntimesTargets
	FILE SpineRuntimesTargets.cmake
	NAMESPACE Skylicht::
	DESTINATION ${SKYLICHT_TARGET_INSTALL_DIR}
)
endif()