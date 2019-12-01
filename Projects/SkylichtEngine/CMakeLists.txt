include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtSystem/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtEngine/Source	
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Source
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Spark/include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/curl/include
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtPhysics/Source
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtAudio/Source	
)

#if (BUILD_SKYLICHT_NETWORK)
#include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtNetwork/Source)
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

add_library(SkylichtEngine STATIC ${skylicht_source})

target_precompiled_header(SkylichtEngine ./Source/pch.cpp ${skylicht_source})

set_target_properties(SkylichtEngine PROPERTIES VERSION ${SKYLICHT_VERSION})

#if(BUILD_RECAST_NAVIGATION_LIB)
#target_link_libraries(SkylichtEngine RecastNavigation)
#endif()

target_link_libraries(SkylichtEngine
	Irrlicht
#	Spark
	SkylichtSystem
	#SkylichtAudio 
#	SkylichtPhysics
)
	
#if (BUILD_SKYLICHT_NETWORK)
#target_link_libraries(SkylichtEngine SkylichtNetwork)
#endif()