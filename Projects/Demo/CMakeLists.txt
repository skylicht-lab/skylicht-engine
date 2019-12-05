include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtSystem/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtClient/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include	
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtEngine/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Demo/Source
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtPhysics/Source
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtAudio/Source
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtComponents/Source
)

#if (BUILD_SKYLICHT_NETWORK)
#include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtNetwork/Source)
#endif()

file(GLOB_RECURSE demo_source 
	./Source/**.cpp 
	./Source/**.c 
	./Source/**.h)
	
add_library(Demo STATIC ${demo_source})

target_precompiled_header(Demo ./Source/pch.cpp ${demo_source})

set_target_properties(Demo PROPERTIES VERSION ${SKYLICHT_VERSION})

target_link_libraries(Demo SkylichtClient)