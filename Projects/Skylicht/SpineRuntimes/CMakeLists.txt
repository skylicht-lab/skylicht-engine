include_directories(
	${SKYLICHT_ENGINE_PROJECT_DIR}/SpineCpp/spine-cpp/include
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/System
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/SpineRuntimes
	${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Include
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Engine
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Components
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Collision
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Physics
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Lightmapper
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Audio
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/GUI
)

if (BUILD_DEBUG_VLD)
	set(vld_inc_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/Vld/include")
	include_directories(${vld_inc_path})

	if (CMAKE_CL_64)
		set(vld_lib_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/Vld/lib/Win64")
	else()
		set(vld_lib_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/Vld/lib/Win32")
	endif()
endif()

if (BUILD_SKYLICHT_NETWORK)
include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Network)
endif()

file(GLOB_RECURSE skylicht_spine_source 
	./**.cpp 
	./**.c 
	./**.h)
	
setup_project_group("${skylicht_spine_source}" ${CMAKE_CURRENT_SOURCE_DIR})
	
add_library(SpineRuntimes ${ENGINE_SHARED_OR_STATIC_LIB} ${skylicht_spine_source})

if (BUILD_SHARED_LIBS)
set_target_properties(SpineRuntimes PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
endif()

target_precompiled_header(SpineRuntimes ./pch.cpp ${skylicht_spine_source})

set_target_properties(SpineRuntimes PROPERTIES VERSION ${SKYLICHT_VERSION})

target_link_libraries(SpineRuntimes Engine spine-cpp)

if (BUILD_DEBUG_VLD)
	target_link_libraries(SpineRuntimes vld)
	target_link_directories(SpineRuntimes PUBLIC ${vld_lib_path})
endif()

if (INSTALL_LIBS)
install(TARGETS SpineRuntimes
			EXPORT SpineRuntimesTargets
			RUNTIME DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)
			
install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/SpineRuntimes
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/Skylicht
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT SpineRuntimesTargets
	FILE SpineRuntimesTargets.cmake
	NAMESPACE Skylicht::
	DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}/cmake
)
endif()