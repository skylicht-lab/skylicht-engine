include_directories(
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Network
	${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Include
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/curl/include
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/System
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Engine
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Crypto
)

file(GLOB_RECURSE skylicht_network 
	./*.cpp 
	./*.hpp 
	./*.c 
	./*.h)
setup_project_group("${skylicht_network}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Network ${ENGINE_SHARED_OR_STATIC_LIB}
	${skylicht_network}
)

if (BUILD_SHARED_LIBS)
set_target_properties(Network PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
endif()

target_precompiled_header(Network ./pch.cpp ${skylicht_network})

set_target_properties(Network PROPERTIES VERSION ${SKYLICHT_VERSION})

if (MSVC OR CYGWIN OR MINGW)
	target_link_libraries(Network ws2_32)
endif()

target_link_libraries(Network Engine System CryptoLib)

if (INSTALL_LIBS)
install(TARGETS Network
			EXPORT NetworkTargets
			RUNTIME DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)

install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Network
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/Skylicht
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT NetworkTargets
	FILE NetworkTargets.cmake
	NAMESPACE Skylicht::
	DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}/cmake
)
endif()