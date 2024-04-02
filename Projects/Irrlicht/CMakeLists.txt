include_directories(
	${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Include
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/source
)

# SDL Include
if(BUILD_SDL OR BUILD_EMSCRIPTEN)
	if(MSVC)
		include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/SDL/include)
	endif()

	if (BUILD_EMSCRIPTEN)
		# See Github Action CI for more details
		include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/InstallLibs)
	endif()

	if(CYGWIN OR MINGW)
		find_file(SDL2_INCLUDE_DIR NAME SDL.h HINTS SDL2)
		include_directories(${SDL2_INCLUDE_DIR})
	endif()
endif()

if(CYGWIN)
	# DirectX11
	add_definitions(-DIRR_COMPILE_WITH_DX11_DEV_PACK)
endif()

file(GLOB irrlicht_source 
	./Source/**.cpp 
	./Source/**.c)
file(GLOB irrlicht_include 
	./Include/**.h)

source_group("Source" FILES "${irrlicht_source}")
source_group("Include" FILES "${irrlicht_include}")

if (BUILD_MACOS)
	# Add .mm source
	file(GLOB irrlicht_mac_source 
		./Source/MacOS/**.mm)
	source_group("Source" FILES "${irrlicht_mac_source}")
endif()

if(BUILD_MACOS OR BUILD_IOS)
	include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Angle/include)
	include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Source/Angle)
	
	# Add angle source
	file(GLOB_RECURSE irrlicht_angle_source
		./Source/Angle/**.mm
		./Source/Angle/**.m
		./Source/Angle/**.cpp
		./Source/Angle/**.h
		./Source/Angle/**.c)
	setup_project_group_by_folder_relative("${irrlicht_angle_source}" "${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Source/Angle")
endif()

add_library(Irrlicht ${ENGINE_SHARED_OR_STATIC_LIB}
	${irrlicht_source}
	${irrlicht_mac_source}
	${irrlicht_angle_source}
	${irrlicht_include})

# Irrlicht shared
if (BUILD_SHARED_LIBS)
	set_target_properties(Irrlicht PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
	add_definitions(-DIRRLICHT_EXPORTS)
endif()

set_target_properties(Irrlicht PROPERTIES VERSION ${SKYLICHT_VERSION})

# ThirdParty
target_link_libraries(Irrlicht ThirdParty)

# Window DirectX11 && OpenGL
if (MSVC OR CYGWIN OR MINGW)
	# OpenGL
	target_link_libraries(Irrlicht opengl32 Winmm)

	# DirectX11 (No DX11 on MINGW)
	if (NOT MINGW)	
	target_link_libraries(Irrlicht d3d11 d3dcompiler dxguid dxgi)
	endif()
endif()

# Linux
if (BUILD_LINUX)
	target_link_libraries(Irrlicht GL)
	target_link_libraries(Irrlicht X11)
endif()

if (BUILD_MACOS)
	target_link_libraries(Irrlicht "-framework Cocoa")
endif()

# Android
if (BUILD_ANDROID)
	# OpenGL ES 3.0
	target_link_libraries(Irrlicht GLESv3)
endif()

# SDL
if(BUILD_SDL)
	if(MSVC)
		target_link_libraries(Irrlicht SDL2)
	elseif(CYGWIN OR MINGW)
		find_library(SDL2_LIBRARY NAME SDL2)
		target_link_libraries(Irrlicht ${SDL2_LIBRARY})
	else()
		target_link_libraries(Irrlicht SDL2)
	endif()	
endif()

if (INSTALL_LIBS)
install(TARGETS Irrlicht
			EXPORT IrrlichtTargets
			RUNTIME DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)

install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Include
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/Irrlicht
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT IrrlichtTargets
	FILE IrrlichtTargets.cmake
	NAMESPACE Skylicht::
	DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}/cmake
)
endif()