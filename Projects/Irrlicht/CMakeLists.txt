include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source
)

# SDL Include
if(BUILD_SDL OR BUILD_EMSCRIPTEN)
	if(MSVC)
		include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdPartySDK/SDL/include)
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

file(GLOB_RECURSE irrlicht_source ./**.cpp ./**.c)
file(GLOB_RECURSE irrlicht_include ./**.h)

source_group("Source" FILES "${irrlicht_source}")
source_group("Include" FILES "${irrlicht_include}")

add_library(Irrlicht STATIC ${irrlicht_source} ${irrlicht_include})

set_target_properties(Irrlicht PROPERTIES VERSION ${SKYLICHT_VERSION})
set_property(TARGET Irrlicht PROPERTY CXX_STANDARD 11)

# ThirdParty
target_link_libraries(Irrlicht ThirdParty)

if (MSVC OR CYGWIN OR MINGW)
	# OpenGL
	target_link_libraries(Irrlicht opengl32 Winmm)

	# DirectX11 (No DX11 on MINGW)
	if (NOT MINGW)	
	target_link_libraries(Irrlicht d3d11 d3dcompiler dxguid dxgi)
	endif()
endif()

# SDL
if(BUILD_SDL)
	if(MSVC)
		target_link_libraries(Irrlicht SDL2)
	elseif(CYGWIN OR MINGW OR BUILD_LINUX)
		find_library(SDL2_LIBRARY NAME SDL2)
		target_link_libraries(Irrlicht ${SDL2_LIBRARY})
	else()
		target_link_libraries(Irrlicht sdl2)
	endif()	
endif()