include_directories(	
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Audio/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/System/Source
)

file(GLOB_RECURSE skylicht_audio_source 
	./Source/*.cpp 
	./Source/*.hpp 
	./Source/*.c 
	./Source/*.h)
setup_project_group("${skylicht_audio_source}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Audio STATIC ${skylicht_audio_source})

target_precompiled_header(Audio ./Source/stdafx.cpp ${skylicht_audio_source})

set_target_properties(Audio PROPERTIES VERSION ${SKYLICHT_VERSION})

if (BUILD_LINUX)
	target_link_libraries(Audio OpenAL)
elseif(BUILD_ANDROID)
	target_link_libraries(Audio OpenSLES)
elseif(BUILD_MACOS)
	target_link_libraries(Audio)
else()
	target_link_libraries(Audio)
endif()