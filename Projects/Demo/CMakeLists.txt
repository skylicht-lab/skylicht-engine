include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Demo/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/System/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Client/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include	
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Engine/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Components/Source
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Physics/Source
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Audio/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/curl/include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/freetype2/include
)

if (BUILD_IMGUI)
	set(imgui_inc_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Imgui/Source")
	include_directories(${imgui_inc_path})
endif()

#if (BUILD_SKYLICHT_NETWORK)
#include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Network/Source)
#endif()

file(GLOB_RECURSE demo_source 
	./Source/**.cpp 
	./Source/**.c 
	./Source/**.h)

setup_project_group("${demo_source}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Demo STATIC ${demo_source})

target_precompiled_header(Demo ./Source/pch.cpp ${demo_source})

set_target_properties(Demo PROPERTIES VERSION ${SKYLICHT_VERSION})

target_link_libraries(Demo Client)

if (BUILD_IMGUI)
	target_link_libraries(Demo Imgui)
endif()