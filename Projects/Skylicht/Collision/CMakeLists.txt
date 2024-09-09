include_directories(
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Collision
	${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Include
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/System
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Engine
)

file(GLOB_RECURSE skylicht_collision 
	./*.cpp 
	./*.hpp 
	./*.c 
	./*.h)
setup_project_group("${skylicht_collision}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Collision ${ENGINE_SHARED_OR_STATIC_LIB}
	${skylicht_collision}
)

if (BUILD_SHARED_LIBS)
	set_target_properties(Collision PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
	add_definitions(-DCOLLISION_EXPORTS)
endif()

target_precompiled_header(Collision ./pch.cpp ${skylicht_collision})

set_target_properties(Collision PROPERTIES VERSION ${SKYLICHT_VERSION})

target_link_libraries(Collision Engine System)

if (INSTALL_LIBS)
install(TARGETS Collision
			EXPORT CollisionTargets
			RUNTIME DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)

install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Collision
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/Skylicht
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT CollisionTargets
	FILE CollisionTargets.cmake
	NAMESPACE Skylicht::
	DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}/cmake
)
endif()