include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/MainApp/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Spark/include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/System/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Engine/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Client/Source
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Audio/Source
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Physics/Source
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
	include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/MainApp/Platforms/Win32)

	file(GLOB_RECURSE platform_win32_source 
		./Platforms/Win32/**.cpp 
		./Platforms/Win32/**.c 
		./Platforms/Win32/**.h)

	list (APPEND main_app_source ${platform_win32_source})
endif()

if (BUILD_MACOS)
	# Angle API	
	include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Angle/include)
	include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/MainApp/Platforms/MacOS)
	
	file(GLOB_RECURSE platform_mac_source 
		./Platforms/MacOS/**.cpp 
		./Platforms/MacOS/**.c 
		./Platforms/MacOS/**.h
		./Platforms/MacOS/**.m
		./Platforms/MacOS/**.mm)

	list (APPEND main_app_source ${platform_mac_source})

	setup_project_group("${platform_mac_source}" ${CMAKE_CURRENT_SOURCE_DIR})
endif()

#if (BUILD_SKYLICHT_NETWORK)
#include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtNetwork/Source)
#endif()

#if(BUILD_RECAST_NAVIGATION_LIB)
#include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/RecastNavigation/Source)
#endif()
	
if (BUILD_DEBUG_VLD)
	if (CMAKE_CL_64)		
		set(vld_lib_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/Vld/lib/Win64")
		set(vld_dll_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/Vld/bin/Win64/*.*")
	else()
		set(vld_lib_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/Vld/lib/Win32")
		set(vld_dll_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/Vld/bin/Win32/*.*")
	endif()	
	link_directories(${vld_lib_path})	
endif()
	
if (MINGW OR CYGWIN)
	# .rc build
	set(CMAKE_RC_COMPILER_INIT windres)
	enable_language(RC)
	set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")    
endif()

if (BUILD_EMSCRIPTEN)
	add_executable(MainApp 
		${main_app_source}
		./Platforms/Emscripten/MainWebGL.cpp
	)
elseif(BUILD_SDL)
	add_executable(MainApp 
		${main_app_source}
		./Platforms/SDL2/MainSDL.cpp
	)
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
elseif (BUILD_MACOS)
	# file(GLOB resources_files "${SKYLICHT_ENGINE_SOURCE_DIR}/Bin/*.zip")

	add_executable(MainApp ${main_app_source} ${resources_files})

	set_target_properties(MainApp PROPERTIES
		MACOSX_BUNDLE TRUE
	)
	
	# foreach(res_file ${resources_files})
	#	file(RELATIVE_PATH res_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Bin" ${res_file})
	#	message(STATUS "- Add resources: ${res_path}")
	#	set_property(SOURCE ${res_file} PROPERTY MACOSX_PACKAGE_LOCATION "Resources")
	#	source_group("Bin" FILES "${res_file}")
	#endforeach()

	# Angle library
	set(angle_lib_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Angle/out/MacOS/Release")
	set(angle_dylib_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Angle/out/MacOS/Release/*.dylib")
	
	target_link_libraries(MainApp "${angle_lib_path}/libEGL.dylib")
	target_link_libraries(MainApp "${angle_lib_path}/libGLESv2.dylib")

	target_link_libraries(MainApp "-framework Cocoa")
else()
	add_executable(MainApp ${main_app_source})
endif()

target_precompiled_header(MainApp ./Source/pch.cpp ${main_app_source})

# Linker
if (BUILD_ANDROID)
	target_link_libraries(MainApp Client Demo log)
else()
	target_link_libraries(MainApp Client Demo)
endif()

# Emscripten
if (BUILD_EMSCRIPTEN)
	message(STATUS "Setting compilation target to WASM")
	set(CMAKE_EXECUTABLE_SUFFIX ".wasm.html")
	set_target_properties(MainApp PROPERTIES LINK_FLAGS "-s USE_SDL=2 -s USE_WEBGL2=1 -s FORCE_FILESYSTEM=1 -s FETCH=1 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_FUNCTIONS=['_main','_main_resize'] -s EXTRA_EXPORTED_RUNTIME_METHODS=['ccall'] --preload-file ../../../Bin/BuiltIn --shell-file shell.html --memory-init-file 1 --disable-shared -s WASM=1 -s BINARYEN_METHOD='native-wasm'")
endif()

#VLD
if (BUILD_DEBUG_VLD)
	file(GLOB_RECURSE vld_bin_files ${vld_dll_path})
	foreach(vld_bin ${vld_bin_files})
		add_custom_command(TARGET MainApp POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${vld_bin} $<TARGET_FILE_DIR:MainApp>)
	endforeach()
endif()

set_target_properties(MainApp PROPERTIES VERSION ${SKYLICHT_VERSION})
set_target_properties(MainApp PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
