include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/MainApp/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Spark/include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtSystem/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtEngine/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtClient/Source
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtAudio/Source
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtPhysics/Source
)

file(GLOB_RECURSE main_app_source 
	./Source/**.cpp 
	./Source/**.c 
	./Source/**.h)

if (BUILD_ANDROID)
file(GLOB_RECURSE platform_android_source 
	./Platforms/Android/**.cpp 
	./Platforms/Android/**.c 
	./Platforms/Android/**.h)

list (APPEND main_app_source ${platform_android_source})
endif()

if (BUILD_WINDOWS_STORE)
file(GLOB_RECURSE platform_uwp_source 
	./Platforms/UWP/**.cpp 
	./Platforms/UWP/**.c 
	./Platforms/UWP/**.h)
file(GLOB_RECURSE platform_uwp_asset ${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Projects/MainApp/Assets/**.*)

list (APPEND main_app_source ${platform_uwp_source})

set_property(SOURCE ${platform_uwp_asset} PROPERTY VS_DEPLOYMENT_CONTENT 1)
endif()

if (MSVC OR CYGWIN OR MINGW)
include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/MainApp/Platforms/Win32
)

file(GLOB_RECURSE platform_win32_source 
	./Platforms/Win32/**.cpp 
	./Platforms/Win32/**.c 
	./Platforms/Win32/**.h)

list (APPEND main_app_source ${platform_win32_source})
endif()

#if (BUILD_SKYLICHT_NETWORK)
#include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtNetwork/Source)
#endif()

#if(BUILD_RECAST_NAVIGATION_LIB)
#include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/RecastNavigation/Source)
#endif()
	
if(MINGW OR CYGWIN)
	# .rc build
	set(CMAKE_RC_COMPILER_INIT windres)
	enable_language(RC)
	set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")    
endif()

if (BUILD_EMSCRIPTEN)
	add_executable(MainApp ${main_app_source})
elseif(BUILD_ANDROID)
	add_library(MainApp SHARED ${main_app_source})
elseif (BUILD_WINDOWS_STORE)
	add_executable(MainApp
		${main_app_source}
		${platform_uwp_asset}
		./Platforms/UWP/MainApp_TemporaryKey.pfx
		./Platforms/UWP/Package.appxmanifest
	)
elseif (MSVC OR CYGWIN OR MINGW)
	add_executable(MainApp WIN32
		${main_app_source}
		./Platforms/Win32/Skylicht.rc
	)	
else()
	add_executable(MainApp ${main_app_source})
endif()

target_precompiled_header(MainApp ./Source/pch.cpp ${main_app_source})

# Linker
if (BUILD_ANDROID)
	target_link_libraries(MainApp SkylichtClient Demo log)
else()
	target_link_libraries(MainApp SkylichtClient Demo)
endif()

# Emscripten
if (BUILD_EMSCRIPTEN)
if(JS_ONLY)
	message(STATUS "Setting compilation target to native JavaScript")
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
	set_target_properties(MainApp PROPERTIES LINK_FLAGS "-s USE_SDL=2 -s USE_WEBGL2=1 -s FORCE_FILESYSTEM=1 -s FETCH=1 -s EXPORTED_FUNCTIONS=['_main'] --preload-file ../../../Bin/BuiltIn --shell-file shell.html --memory-init-file 1 --disable-shared")
else()
	message(STATUS "Setting compilation target to WASM")
	set(CMAKE_EXECUTABLE_SUFFIX ".wasm.html")
	set_target_properties(MainApp PROPERTIES LINK_FLAGS "-s USE_SDL=2 -s USE_WEBGL2=1 -s FORCE_FILESYSTEM=1 -s FETCH=1 -s EXPORTED_FUNCTIONS=['_main'] --preload-file ../../../Bin/BuiltIn --shell-file shell.html --memory-init-file 1 --disable-shared -s WASM=1 -s BINARYEN_METHOD='native-wasm'")
endif()
endif()

set_target_properties(MainApp PROPERTIES VERSION ${SKYLICHT_VERSION})
set_target_properties(MainApp PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")