include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/UserInterface/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/curl/include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Engine/Source
)

file(GLOB_RECURSE skylicht_ui 
	./Source/*.cpp 
	./Source/*.hpp 
	./Source/*.c 
	./Source/*.h)
setup_project_group("${skylicht_ui}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(UserInterface STATIC
	${skylicht_ui}
)

target_precompiled_header(UserInterface ./Source/pch.cpp ${skylicht_ui})

set_target_properties(UserInterface PROPERTIES VERSION ${SKYLICHT_VERSION})

target_link_libraries(UserInterface Engine)

if (INSTALL_LIBS)
install(TARGETS UserInterface
			RUNTIME DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)
endif()