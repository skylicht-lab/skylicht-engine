include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Crypto/Source
)

file(GLOB_RECURSE crypto_source 
	./Source/*.cpp 
	./Source/*.hpp 
	./Source/*.c 
	./Source/*.h)
setup_project_group("${crypto_source}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(CryptoLib ${ENGINE_SHARED_OR_STATIC_LIB} ${crypto_source})

if (BUILD_SHARED_LIBS)
set_target_properties(CryptoLib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
endif()

set_target_properties(CryptoLib PROPERTIES VERSION ${SKYLICHT_VERSION})

if (INSTALL_LIBS)
install(TARGETS CryptoLib
			EXPORT CryptoLibTargets
			RUNTIME DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)
			
install (DIRECTORY ${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Crypto/Source
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/Skylicht/Crypto
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT CryptoLibTargets
	FILE CryptoLibTargets.cmake
	NAMESPACE Skylicht::
	DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}/cmake
)
endif()