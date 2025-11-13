include_directories(
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/System
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Client
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

if (BUILD_SKYLICHT_UI)
include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/UserInterface)
endif()

file(GLOB_RECURSE skylicht_client_source 
	./**.cpp 
	./**.c 
	./**.h)
	
setup_project_group("${skylicht_client_source}" ${CMAKE_CURRENT_SOURCE_DIR})
	
add_library(Client ${ENGINE_SHARED_OR_STATIC_LIB} ${skylicht_client_source})

if (BUILD_SHARED_LIBS)
set_target_properties(Client PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
endif()

target_precompiled_header(Client ./pch.cpp ${skylicht_client_source})

set_target_properties(Client PROPERTIES VERSION ${SKYLICHT_VERSION})

target_link_libraries(Client Engine)

if (BUILD_SKYLICHT_COMPONENTS)
	target_link_libraries(Client Components)
endif()

if (BUILD_SKYLICHT_COLLISION)
	target_link_libraries(Client Collision)
endif()

if (BUILD_SKYLICHT_PHYSIC)
	target_link_libraries(Client Physics)
endif()

if (BUILD_SKYLICHT_LIGHMAPPER)
	target_link_libraries(Client Lightmapper)
endif()

if (BUILD_SKYLICHT_UI)
	target_link_libraries(Client UserInterface)
endif()

if (BUILD_SKYLICHT_NETWORK)
	target_link_libraries(Client Network)
endif()

if (BUILD_SKYLICHT_GRAPH)
	target_link_libraries(Client Graph)
endif()

if (BUILD_DEBUG_VLD)
	target_link_libraries(Client vld)
	target_link_directories(Client PUBLIC ${vld_lib_path})
endif()

if (INSTALL_LIBS)
install(TARGETS Client
			EXPORT ClientTargets
			RUNTIME DESTINATION ${SKYLICHT_RUNTIME_INSTALL_DIR}
			LIBRARY DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			ARCHIVE DESTINATION ${SKYLICHT_ARCHIVE_INSTALL_DIR})
			
install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Client
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/Skylicht
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT ClientTargets
	FILE ClientTargets.cmake
	NAMESPACE Skylicht::
	DESTINATION ${SKYLICHT_TARGET_INSTALL_DIR}
)
endif()