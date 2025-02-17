include_directories(
	${SKYLICHT_ENGINE_PROJECT_DIR}/Crypto
)

file(GLOB_RECURSE crypto_source 
	./*.cpp 
	./*.hpp 
	./*.c 
	./*.h)
setup_project_group("${crypto_source}" ${CMAKE_CURRENT_SOURCE_DIR})

if (BUILD_SHARED_LIBS)
if (BUILD_LINUX OR BUILD_MACOS)
add_compile_options(-fpic)
endif()
endif()

add_library(CryptoLib STATIC ${crypto_source})

set_target_properties(CryptoLib PROPERTIES VERSION ${SKYLICHT_VERSION})

if (INSTALL_LIBS)
install(TARGETS CryptoLib
			EXPORT CryptoLibTargets
			RUNTIME DESTINATION ${SKYLICHT_RUNTIME_INSTALL_DIR}
			LIBRARY DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			ARCHIVE DESTINATION ${SKYLICHT_ARCHIVE_INSTALL_DIR})
			
install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Crypto
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/Skylicht
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT CryptoLibTargets
	FILE CryptoLibTargets.cmake
	NAMESPACE Skylicht::
	DESTINATION ${SKYLICHT_TARGET_INSTALL_DIR}
)
endif()