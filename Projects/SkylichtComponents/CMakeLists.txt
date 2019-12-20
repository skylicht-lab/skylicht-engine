include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtComponents/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Source	
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Spark/include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/curl/include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtSystem/Source
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtAudio/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtEngine/Source
)

file(GLOB_RECURSE skylicht_components 
	./Source/*.cpp 
	./Source/*.hpp 
	./Source/*.c 
	./Source/*.h)
setup_project_group("${skylicht_components}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(SkylichtComponents STATIC
	${skylicht_components}
)

target_precompiled_header(SkylichtComponents ./Source/pch.cpp ${skylicht_components})

set_target_properties(SkylichtComponents PROPERTIES VERSION ${SKYLICHT_VERSION})

if (BUILD_EMSCRIPTEN)
target_link_libraries(SkylichtComponents SkylichtEngine SkylichtSystem)
elseif(MSVC)
target_link_libraries(SkylichtComponents SkylichtEngine SkylichtSystem)
elseif(CYGWIN OR MINGW)
target_link_libraries(SkylichtComponents SkylichtEngine SkylichtSystem)
endif()