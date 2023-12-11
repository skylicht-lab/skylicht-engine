include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/System/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Client/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Engine/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Components/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Collision/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Physics/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Lightmapper/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Audio/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/GUI/Source
)

if (BUILD_DEBUG_VLD)
	set(vld_inc_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/Vld/include")
	include_directories(${vld_inc_path})
endif()

if (BUILD_SKYLICHT_NETWORK)
include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Network/Source)
endif()

file(GLOB_RECURSE skylicht_client_source 
	./Source/**.cpp 
	./Source/**.c 
	./Source/**.h)
	
setup_project_group("${skylicht_client_source}" ${CMAKE_CURRENT_SOURCE_DIR})
	
add_library(Client STATIC ${skylicht_client_source})

target_precompiled_header(Client ./Source/pch.cpp ${skylicht_client_source})

set_target_properties(Client PROPERTIES VERSION ${SKYLICHT_VERSION})

target_link_libraries(Client Engine Components Collision Physics Lightmapper GUI)

if (BUILD_SKYLICHT_NETWORK)
	target_link_libraries(Client Network)
endif()

if (BUILD_DEBUG_VLD)
	target_link_libraries(Client vld)
endif()