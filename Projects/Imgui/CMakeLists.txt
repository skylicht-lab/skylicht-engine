include_directories(	
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/System/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Engine/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Imgui/Source
)

file(GLOB_RECURSE imgui_source
	./Source/**.cpp
	./Source/**.c 
	./Source/**.h)

setup_project_group("${imgui_source}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Imgui STATIC ${imgui_source})

set_target_properties(Imgui PROPERTIES VERSION ${SKYLICHT_VERSION})

target_link_libraries(Imgui Engine Irrlicht)