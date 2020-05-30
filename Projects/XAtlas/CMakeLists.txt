include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/XAtlas/Source
)

file(GLOB_RECURSE xatlas_source 
	./Source/*.cpp 
	./Source/*.hpp 
	./Source/*.c 
	./Source/*.h)
setup_project_group("${xatlas_source}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(XAtlas STATIC
	${xatlas_source}
)

set_target_properties(XAtlas PROPERTIES VERSION ${SKYLICHT_VERSION})