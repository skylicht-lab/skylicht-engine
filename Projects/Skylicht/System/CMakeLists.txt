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

add_library(System STATIC
	${skylicht_system_source}
)

set_target_properties(System PROPERTIES VERSION ${SKYLICHT_VERSION})

if (NOT MSVC AND NOT BUILD_ANDROID AND NOT BUILD_EMSCRIPTEN)
target_link_libraries(System ThirdParty pthread)
endif()

if (INSTALL_LIBS)
install(TARGETS System
			RUNTIME DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)

install (DIRECTORY ${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/System/Source
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/System
	FILES_MATCHING PATTERN "*.h*")
endif()