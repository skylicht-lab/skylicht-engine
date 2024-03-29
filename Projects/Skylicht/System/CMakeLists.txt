include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/System/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source
)

file(GLOB_RECURSE skylicht_system_source 
	./Source/*.cpp 
	./Source/*.hpp 
	./Source/*.c 
	./Source/*.h)
setup_project_group("${skylicht_system_source}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(System ${ENGINE_SHARED_OR_STATIC_LIB}
	${skylicht_system_source}
)

if (BUILD_SHARED_LIBS)
	set_target_properties(System PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
	add_definitions(-DSYSTEM_EXPORTS)
endif()

set_target_properties(System PROPERTIES VERSION ${SKYLICHT_VERSION})

if (NOT MSVC AND NOT BUILD_ANDROID AND NOT BUILD_EMSCRIPTEN)
target_link_libraries(System ThirdParty pthread)
endif()

if (INSTALL_LIBS)
install(TARGETS System
			EXPORT SystemTargets
			RUNTIME DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)

install (DIRECTORY ${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/System/Source
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/Skylicht/System
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT SystemTargets
	FILE SystemTargets.cmake
	NAMESPACE Skylicht::
	DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}/cmake
)
endif()