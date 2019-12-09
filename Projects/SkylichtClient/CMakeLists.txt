include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtSystem/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtClient/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include	
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtEngine/Source	
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtPhysics/Source
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtAudio/Source
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtComponents/Source
)

#if(BUILD_DEBUG_VLD)
#include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdPartySDK/Vld)
#endif()

#if (BUILD_SKYLICHT_NETWORK)
#include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtNetwork/Source)
#endif()

#if(BUILD_RECAST_NAVIGATION_LIB)
#include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/RecastNavigation/Source)
#endif()

file(GLOB_RECURSE skylicht_client_source 
	./Source/**.cpp 
	./Source/**.c 
	./Source/**.h)
	
add_library(SkylichtClient STATIC ${skylicht_client_source})

target_precompiled_header(SkylichtClient ./Source/pch.cpp ${skylicht_client_source})

set_target_properties(SkylichtClient PROPERTIES VERSION ${SKYLICHT_VERSION})

target_link_libraries(SkylichtClient SkylichtEngine)