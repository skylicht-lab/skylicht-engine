include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Network/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/curl/include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/System/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Audio/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Engine/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Crypto/Source
)

file(GLOB_RECURSE skylicht_network 
	./Source/*.cpp 
	./Source/*.hpp 
	./Source/*.c 
	./Source/*.h)
setup_project_group("${skylicht_network}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Network STATIC
	${skylicht_network}
)

target_precompiled_header(Network ./Source/pch.cpp ${skylicht_network})

set_target_properties(Network PROPERTIES VERSION ${SKYLICHT_VERSION})

if (MSVC OR CYGWIN OR MINGW)	
	target_link_libraries(Network ws2_32)
else()
	target_link_libraries(Network socket)
endif()

if (BUILD_EMSCRIPTEN)
target_link_libraries(Network Engine System)
elseif(MSVC)
target_link_libraries(Network Engine System)
elseif(CYGWIN OR MINGW)
target_link_libraries(Network Engine System)
endif()