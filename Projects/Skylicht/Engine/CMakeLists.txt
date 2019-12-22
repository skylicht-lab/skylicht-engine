include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/System/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Engine/Source	
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Source
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Spark/include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/curl/include
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Physics/Source
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Audio/Source	
)

#if (BUILD_SKYLICHT_NETWORK)
#include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Network/Source)
#endif()

#if(BUILD_RECAST_NAVIGATION_LIB)
#include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/RecastNavigation/Source)
#endif()

file(GLOB_RECURSE skylicht_source 
	./Source/*.cpp 
	./Source/*.hpp 
	./Source/*.c 
	./Source/*.h)
setup_project_group("${skylicht_source}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Engine STATIC ${skylicht_source})

target_precompiled_header(Engine ./Source/pch.cpp ${skylicht_source})

set_target_properties(Engine PROPERTIES VERSION ${SKYLICHT_VERSION})

#if(BUILD_RECAST_NAVIGATION_LIB)
#target_link_libraries(Engine RecastNavigation)
#endif()

target_link_libraries(Engine
	Irrlicht
#	Spark
	System
#	Audio 
#	Physics
)
	
#if (BUILD_SKYLICHT_NETWORK)
#target_link_libraries(Engine Skylicht/Network)
#endif()