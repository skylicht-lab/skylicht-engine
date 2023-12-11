include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/GUI/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/curl/include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Engine/Source
)

file(GLOB_RECURSE skylicht_gui 
	./Source/*.cpp 
	./Source/*.hpp 
	./Source/*.c 
	./Source/*.h)
setup_project_group("${skylicht_gui}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(GUI STATIC
	${skylicht_gui}
)

target_precompiled_header(GUI ./Source/pch.cpp ${skylicht_gui})

set_target_properties(GUI PROPERTIES VERSION ${SKYLICHT_VERSION})

target_link_libraries(GUI Engine)