include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Components/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/curl/include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/System/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Audio/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Engine/Source
)

file(GLOB_RECURSE skylicht_components 
	./Source/*.cpp 
	./Source/*.hpp 
	./Source/*.c 
	./Source/*.h)
setup_project_group("${skylicht_components}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Components STATIC
	${skylicht_components}
)

target_precompiled_header(Components ./Source/pch.cpp ${skylicht_components})

set_target_properties(Components PROPERTIES VERSION ${SKYLICHT_VERSION})

if (BUILD_EMSCRIPTEN)
target_link_libraries(Components Engine System)
elseif(MSVC)
target_link_libraries(Components Engine System)
elseif(CYGWIN OR MINGW)
target_link_libraries(Components Engine System)
endif()