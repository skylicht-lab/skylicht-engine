include_directories(
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Components/Source
	${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Include
	${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Source
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/System/Source
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Engine/Source
)

file(GLOB_RECURSE skylicht_components 
	./Source/*.cpp 
	./Source/*.hpp 
	./Source/*.c 
	./Source/*.h)
setup_project_group("${skylicht_components}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Components ${ENGINE_SHARED_OR_STATIC_LIB}
	${skylicht_components}
)

if (BUILD_SHARED_LIBS)
	set_target_properties(Components PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
	add_definitions(-DCOMPONENTS_EXPORTS)
endif()

target_precompiled_header(Components ./Source/pch.cpp ${skylicht_components})

set_target_properties(Components PROPERTIES VERSION ${SKYLICHT_VERSION})

target_link_libraries(Components Engine System)

if (INSTALL_LIBS)
install(TARGETS Components
			EXPORT ComponentsTargets
			RUNTIME DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)

install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Components/Source
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/Skylicht/Components
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT ComponentsTargets
	FILE ComponentsTargets.cmake
	NAMESPACE Skylicht::
	DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}/cmake
)
endif()