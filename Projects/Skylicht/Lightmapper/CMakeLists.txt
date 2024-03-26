include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/System/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Lightmapper/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include	
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Engine/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Components/Source
)

file(GLOB_RECURSE skylicht_lightmapper_source 
	./Source/**.cpp 
	./Source/**.c 
	./Source/**.h)
	
setup_project_group("${skylicht_lightmapper_source}" ${CMAKE_CURRENT_SOURCE_DIR})
	
add_library(Lightmapper STATIC ${skylicht_lightmapper_source})

target_precompiled_header(Lightmapper ./Source/pch.cpp ${skylicht_lightmapper_source})

set_target_properties(Lightmapper PROPERTIES VERSION ${SKYLICHT_VERSION})

target_link_libraries(Lightmapper Engine Components)

if (INSTALL_LIBS)
install(TARGETS Lightmapper
			RUNTIME DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)
endif()