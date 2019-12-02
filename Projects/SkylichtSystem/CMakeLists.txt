include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtSystem/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source
)

file(GLOB_RECURSE skylicht_system_source 
	./Source/*.cpp 
	./Source/*.hpp 
	./Source/*.c 
	./Source/*.h)
setup_project_group("${skylicht_system_source}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(SkylichtSystem STATIC
	${skylicht_system_source}
)

set_target_properties(SkylichtSystem PROPERTIES VERSION ${SKYLICHT_VERSION})

if (NOT MSVC AND NOT BUILD_ANDROID)
target_link_libraries(SkylichtSystem ThirdParty pthread)
endif()