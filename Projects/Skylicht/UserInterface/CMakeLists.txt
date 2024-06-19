include_directories(
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/UserInterface/Source
	${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Include
	${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Source
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Engine/Source
)

file(GLOB_RECURSE skylicht_ui 
	./Source/*.cpp 
	./Source/*.hpp 
	./Source/*.c 
	./Source/*.h)
setup_project_group("${skylicht_ui}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(UserInterface ${ENGINE_SHARED_OR_STATIC_LIB}
	${skylicht_ui}
)

if (BUILD_SHARED_LIBS)
set_target_properties(UserInterface PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
endif()

target_precompiled_header(UserInterface ./Source/pch.cpp ${skylicht_ui})

set_target_properties(UserInterface PROPERTIES VERSION ${SKYLICHT_VERSION})

target_link_libraries(UserInterface Engine)

if (INSTALL_LIBS)
install(TARGETS UserInterface
			EXPORT UserInterfaceTargets
			RUNTIME DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)

install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/UserInterface/Source
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/Skylicht/UserInterface
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT UserInterfaceTargets
	FILE UserInterfaceTargets.cmake
	NAMESPACE Skylicht::
	DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}/cmake
)
endif()