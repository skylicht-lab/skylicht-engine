include_directories(
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/System
	${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Include	
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Engine
	${SKYLICHT_ENGINE_PROJECT_DIR}/Recast/Include
)

file(GLOB_RECURSE skylicht_graph_source 
	./**.cpp 
	./**.c 
	./**.h)
	
setup_project_group("${skylicht_graph_source}" ${CMAKE_CURRENT_SOURCE_DIR})
	
add_library(Graph ${ENGINE_SHARED_OR_STATIC_LIB} ${skylicht_graph_source})

if (BUILD_SHARED_LIBS)
set_target_properties(Graph PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
endif()

set_target_properties(Graph PROPERTIES VERSION ${SKYLICHT_VERSION})

target_link_libraries(Graph Engine Recast)

if (INSTALL_LIBS)
install(TARGETS Graph
			EXPORT GraphTargets
			RUNTIME DESTINATION ${SKYLICHT_RUNTIME_INSTALL_DIR}
			LIBRARY DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			ARCHIVE DESTINATION ${SKYLICHT_ARCHIVE_INSTALL_DIR})
			
install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Graph
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/Skylicht
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT GraphTargets
	FILE GraphTargets.cmake
	NAMESPACE Skylicht::
	DESTINATION ${SKYLICHT_TARGET_INSTALL_DIR}
)
endif()