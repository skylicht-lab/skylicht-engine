include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Crypto/Source
)

file(GLOB_RECURSE crypto_source 
	./Source/*.cpp 
	./Source/*.hpp 
	./Source/*.c 
	./Source/*.h)
setup_project_group("${crypto_source}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Crypto STATIC ${crypto_source})

set_target_properties(Crypto PROPERTIES VERSION ${SKYLICHT_VERSION})