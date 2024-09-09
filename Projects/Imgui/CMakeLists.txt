include_directories(
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/System
	${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Include
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Engine
	${SKYLICHT_ENGINE_PROJECT_DIR}/Imgui
)

file(GLOB_RECURSE imgui_source
	./**.cpp
	./**.c 
	./**.h)

setup_project_group("${imgui_source}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Imgui STATIC ${imgui_source})

set_target_properties(Imgui PROPERTIES VERSION ${SKYLICHT_VERSION})

target_link_libraries(Imgui Engine Irrlicht)

if (INSTALL_LIBS)
install(TARGETS Imgui
			EXPORT ImguiTargets
			RUNTIME DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)
			
install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/Imgui
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT ImguiTargets
	FILE ImguiTargets.cmake
	NAMESPACE Skylicht::
	DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}/cmake
)
endif()