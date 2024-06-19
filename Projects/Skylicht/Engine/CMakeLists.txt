include_directories(
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/System/Source
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Engine/Source
	${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Include
	${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Source
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/source/freetype2/include
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/source/kdtree
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Audio/Source
)

file(GLOB_RECURSE skylicht_source 
	./Source/*.cpp 
	./Source/*.hpp 
	./Source/*.c 
	./Source/*.h)
setup_project_group("${skylicht_source}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Engine ${ENGINE_SHARED_OR_STATIC_LIB} ${skylicht_source})

if (BUILD_SHARED_LIBS)
	set_target_properties(Engine PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
	add_definitions(-DSKYLICHT_EXPORTS)
endif()

target_precompiled_header(Engine ./Source/pch.cpp ${skylicht_source})

set_target_properties(Engine PROPERTIES VERSION ${SKYLICHT_VERSION})

target_link_libraries(Engine Irrlicht System)

if (BUILD_SKYLICHT_AUDIO)
target_link_libraries(Engine Audio)
endif()

if (INSTALL_LIBS)
install(TARGETS Engine
			EXPORT EngineTargets
			RUNTIME DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)

install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Engine/Source
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/Skylicht/Engine
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT EngineTargets
	FILE EngineTargets.cmake
	NAMESPACE Skylicht::
	DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}/cmake
)
endif()