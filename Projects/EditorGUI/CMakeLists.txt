include_directories(
	${SKYLICHT_ENGINE_PROJECT_DIR}/EditorGUI
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/System
	${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Include
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Engine
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/freetype2/include
)

file(GLOB_RECURSE editorui_source
	./**.cpp
	./**.c 
	./**.h)

setup_project_group("${editorui_source}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(EditorGUI ${ENGINE_SHARED_OR_STATIC_LIB} ${editorui_source})

target_precompiled_header(EditorGUI ./pch.cpp ${editorui_source})

if (BUILD_SHARED_LIBS)
	set_target_properties(EditorGUI PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
	add_definitions(-DEDITOR_GUI_EXPORTS)
endif()

set_target_properties(EditorGUI PROPERTIES VERSION ${SKYLICHT_VERSION})

target_link_libraries(EditorGUI Engine Irrlicht)

if (INSTALL_LIBS)
install(TARGETS EditorGUI
			EXPORT EditorGUITargets
			RUNTIME DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)
			
install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/EditorGUI
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT EditorGUITargets
	FILE EditorGUITargets.cmake
	NAMESPACE Skylicht::
	DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}/cmake
)
endif()