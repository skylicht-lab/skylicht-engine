include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Samples/PBR/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Samples/SampleFW
	${SKYLICHT_ENGINE_PROJECT_DIR}/Main/Source
	${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Include
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/System/Source
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Engine/Source
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Components/Source
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Collision/Source
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Physics/Source
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Client/Source
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Lightmapper/Source
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Audio/Source
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/source/freetype2/include
	${SKYLICHT_ENGINE_PROJECT_DIR}/Bullet3/src
)

if (BUILD_IMGUI)
	include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Imgui/Source)
endif()

set(template_path ${SKYLICHT_ENGINE_PROJECT_DIR}/Main)

file(GLOB_RECURSE application_source 
	./Source/**.cpp 
	./Source/**.c 
	./Source/**.h
	../SampleFW/**.cpp
	../SampleFW/**.h)

setup_project_group("${application_source}" ${CMAKE_CURRENT_SOURCE_DIR})

list (APPEND application_source ${template_path}/Source/pch.cpp)
list (APPEND application_source ${template_path}/Source/pch.h)

if (BUILD_ANDROID)
	file(GLOB_RECURSE platform_android_source 
		${template_path}/Platforms/Android/**.cpp 
		${template_path}/Platforms/Android/**.c 
		${template_path}/Platforms/Android/**.h)

	list (APPEND application_source ${platform_android_source})
endif()

if (BUILD_WINDOWS_STORE)
	file(MAKE_DIRECTORY ${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/PBR)
	file(MAKE_DIRECTORY ${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/PBR/Assets)
	
	file(GLOB uwp_assets 
		${template_path}/Platforms/UWP/Assets/**.*
		${SKYLICHT_ENGINE_BIN_DIR}/**.zip)
	
	foreach(asset_file ${uwp_assets})
		file(COPY "${asset_file}" DESTINATION ${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/PBR/Assets)
	endforeach()

	file(COPY ${template_path}/Platforms/UWP/Package.appxmanifest
		DESTINATION 
		${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/PBR)
		
	file(COPY ${template_path}/Platforms/UWP/TemporaryKey.pfx
		DESTINATION 
		${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/PBR)

	file(GLOB_RECURSE platform_uwp_source 
		${template_path}/Platforms/UWP/**.cpp 
		${template_path}/Platforms/UWP/**.c 
		${template_path}/Platforms/UWP/**.h)
	
	# copy resource to generate prj folder
	file(GLOB_RECURSE platform_uwp_asset ${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/PBR/Assets/**.*)
	add_source_group("${platform_uwp_asset}" "Assets")
	
	list (APPEND application_source ${platform_uwp_source})

	set_property(SOURCE ${platform_uwp_asset} PROPERTY VS_DEPLOYMENT_CONTENT 1)
endif()

if (MSVC OR CYGWIN OR MINGW)
	include_directories(${template_path}/Platforms/Win32)

	file(GLOB_RECURSE platform_win32_source 
		${template_path}/Platforms/Win32/**.cpp 
		${template_path}/Platforms/Win32/**.c 
		${template_path}/Platforms/Win32/**.h)

	list (APPEND application_source ${platform_win32_source})
endif()

if (BUILD_MACOS)
	# Angle API
	include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Angle/include)
	include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Source/Angle)
	include_directories(${template_path}/Platforms/MacOS)
	
	file(GLOB_RECURSE platform_mac_source 
		${template_path}/Platforms/MacOS/**.cpp 
		${template_path}/Platforms/MacOS/**.c 
		${template_path}/Platforms/MacOS/**.h
		${template_path}/Platforms/MacOS/**.m
		${template_path}/Platforms/MacOS/**.mm)

	list (APPEND application_source ${platform_mac_source})
endif()

if (BUILD_IOS)
	# Angle API
	include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Angle/include)
	include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Source/Angle)
	include_directories(${template_path}/Platforms/IOS)
	
	file(GLOB_RECURSE platform_ios_source 
		${template_path}/Platforms/IOS/**.cpp 
		${template_path}/Platforms/IOS/**.c 
		${template_path}/Platforms/IOS/**.h
		${template_path}/Platforms/IOS/**.m
		${template_path}/Platforms/IOS/**.mm)

	file(GLOB_RECURSE platform_ios_resources ${template_path}/Platforms/IOS/**.storyboard)

	list (APPEND application_source ${platform_ios_source})
endif()

if (BUILD_DEBUG_VLD)
	if (CMAKE_CL_64)
		set(vld_dll_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/Vld/bin/Win64/*.*")
	else()
		set(vld_dll_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/Vld/bin/Win32/*.*")
	endif()
endif()
	
if (BUILD_SDL AND MSVC)
	if (CMAKE_CL_64)
		set(sdl_lib_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/SDL/lib/x64")
		set(sdl_dll_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/SDL/lib/x64/*.dll")
	else()
		set(sdl_lib_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/SDL/lib/x86")
		set(sdl_dll_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/SDL/lib/x86/*.dll")
	endif()	
	link_directories(${sdl_lib_path})
endif()	
	
if (MINGW OR CYGWIN)
	# .rc build
	set(CMAKE_RC_COMPILER_INIT windres)
	enable_language(RC)
	set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")    
endif()

if (BUILD_EMSCRIPTEN)
	add_executable(SamplePBR 
		${application_source}
		${template_path}/Platforms/Emscripten/MainWebGL.cpp
	)
elseif(BUILD_SDL)
	add_executable(SamplePBR 
		${application_source}
		${template_path}/Platforms/SDL2/MainSDL.cpp
	)
elseif(BUILD_LINUX)
	add_executable(SamplePBR 
		${application_source}
		${template_path}/Platforms/Linux/MainLinux.cpp
	)
elseif(BUILD_ANDROID)
	add_library(SamplePBR SHARED ${application_source})
elseif (BUILD_WINDOWS_STORE)
	add_executable(SamplePBR
		${application_source}
		${platform_uwp_asset}
		${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/PBR/TemporaryKey.pfx
		${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/PBR/Package.appxmanifest
	)
elseif (MSVC OR CYGWIN OR MINGW)
	add_executable(SamplePBR WIN32
		${application_source}
		${template_path}/Platforms/Win32/Skylicht.rc
	)
elseif (BUILD_MACOS)
	file(GLOB resources_files "${SKYLICHT_ENGINE_BIN_DIR}/*.zip")

	set(angle_lib_path "${SKYLICHT_ENGINE_PROJECT_DIR}/Angle/out/MacOS/Release/${CMAKE_OSX_ARCHITECTURES}")
	file(GLOB dylib_files "${angle_lib_path}/*.dylib")

	add_executable(SamplePBR 
		${application_source} 
		${resources_files}
		${dylib_files}
	)
	
	set_target_properties(SamplePBR PROPERTIES MACOSX_BUNDLE TRUE)
	
	target_link_libraries(SamplePBR "-framework Cocoa")
		
	message(STATUS "- Setup project: SamplePBR")
	foreach(res_file ${resources_files})
		file(RELATIVE_PATH res_path "${SKYLICHT_ENGINE_BIN_DIR}" ${res_file})
		message(STATUS "  - Add resources: ${res_path}")
		set_property(SOURCE ${res_file} PROPERTY MACOSX_PACKAGE_LOCATION "Resources")
		source_group("Bin" FILES "${res_file}")
	endforeach()

	foreach(lib_file ${dylib_files})
		set_property(SOURCE ${lib_file} PROPERTY MACOSX_PACKAGE_LOCATION "Frameworks")
		set_source_files_properties(${lib_file} PROPERTIES XCODE_FILE_ATTRIBUTES "CodeSignOnCopy")
		source_group("Libs" FILES "${lib_file}")
	endforeach()

	add_custom_command(TARGET SamplePBR 
		POST_BUILD COMMAND 
		${CMAKE_INSTALL_NAME_TOOL} -change ./libEGL.dylib @rpath/libEGL.dylib
		$<TARGET_FILE:SamplePBR>)

	add_custom_command(TARGET SamplePBR 
		POST_BUILD COMMAND 
		${CMAKE_INSTALL_NAME_TOOL} -change ./libGLESv2.dylib @rpath/libGLESv2.dylib
		$<TARGET_FILE:SamplePBR>)

	set_target_properties(SamplePBR PROPERTIES
		MACOSX_RPATH ON
		BUILD_WITH_INSTALL_RPATH 1
		INSTALL_RPATH "@executable_path/../Frameworks"
	)
elseif (BUILD_IOS)
	set(MACOSX_BUNDLE_EXECUTABLE_NAME SamplePBR)

	set(APP_NAME "SamplePBR")
	set(APP_BUNDLE_IDENTIFIER "com.team_name.project_name")
	set(CODE_SIGN_IDENTITY "iPhone Developer")

	set(PRODUCT_NAME ${APP_NAME})
	set(EXECUTABLE_NAME ${APP_NAME})
	set(MACOSX_BUNDLE_EXECUTABLE_NAME ${APP_NAME})
	set(MACOSX_BUNDLE_BUNDLE_NAME ${APP_BUNDLE_IDENTIFIER})
	set(MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_BUNDLE_IDENTIFIER})

	file(GLOB resources_files "${SKYLICHT_ENGINE_BIN_DIR}/*.zip")

	set(angle_lib_path "${SKYLICHT_ENGINE_PROJECT_DIR}/Angle/out/IOS/Release/${IOS_PLATFORM}")
	file(GLOB angle_framework_files "${angle_lib_path}/*.framework")

	set (framework_files "")
	list (APPEND framework_files ${angle_framework_files})

	add_executable(SamplePBR MACOSX_BUNDLE 
		${application_source}
		${resources_files}
		${platform_ios_resources}
		${framework_files}
	)
	
	message(STATUS "- Setup project: SamplePBR")
	foreach(res_file ${resources_files})
		file(RELATIVE_PATH res_path "${SKYLICHT_ENGINE_BIN_DIR}" ${res_file})
		source_group("Bin" FILES "${res_file}")
	endforeach()

	foreach(lib_file ${framework_files})
		source_group("Frameworks" FILES "${lib_file}")
		set_property(SOURCE ${lib_file} PROPERTY MACOSX_PACKAGE_LOCATION "Frameworks")
		set_source_files_properties(${lib_file} PROPERTIES XCODE_FILE_ATTRIBUTES "CodeSignOnCopy")
	endforeach()

	set_target_properties(SamplePBR PROPERTIES XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER ${APP_BUNDLE_IDENTIFIER})
	set_target_properties(SamplePBR PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ${CODE_SIGN_IDENTITY})
	set_target_properties(SamplePBR PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY ${DEVICE_FAMILY})
	set_target_properties(SamplePBR PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${template_path}/Platforms/IOS/plist.in")
	
	# add assets icon
	target_sources(SamplePBR PRIVATE "${template_path}/Platforms/IOS/Assets.xcassets")
	
	list (APPEND platform_ios_resources "${template_path}/Platforms/IOS/Assets.xcassets")
	set_target_properties(SamplePBR PROPERTIES RESOURCE "${resources_files} ${platform_ios_resources}")

	# link framework
	target_link_libraries(SamplePBR "-framework Foundation")
	target_link_libraries(SamplePBR "-framework UIKit")
	target_link_libraries(SamplePBR "-framework CoreGraphics")
	target_link_libraries(SamplePBR "-framework Metal")
	target_link_libraries(SamplePBR "-framework MetalKit")
	target_link_libraries(SamplePBR "-framework AudioToolbox")
	target_link_libraries(SamplePBR "-framework AVFAudio")
else()
	add_executable(SamplePBR ${application_source})
endif()

target_precompiled_header(SamplePBR ${template_path}/Source/pch.cpp ${application_source})

# Linker
if (BUILD_ANDROID)
	target_link_libraries(SamplePBR Client log)
else()
	target_link_libraries(SamplePBR Client)
endif()

# Imgui
if (BUILD_IMGUI)
	target_link_libraries(SamplePBR Imgui)
endif()

# Emscripten
if (BUILD_EMSCRIPTEN)
	message(STATUS "Setting build in data: ${CMAKE_CURRENT_BINARY_DIR}")
	file(COPY "${SKYLICHT_ENGINE_BIN_DIR}/BuiltIn" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
	
	message(STATUS "Setting compilation target to WASM")
	set(CMAKE_EXECUTABLE_SUFFIX ".wasm.html")
	set_target_properties(SamplePBR PROPERTIES LINK_FLAGS "-s USE_SDL=2 -s USE_WEBGL2=1 -s FORCE_FILESYSTEM=1 -s FETCH=1 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_FUNCTIONS=['_main','_main_resize'] -s EXPORTED_RUNTIME_METHODS=['ccall'] --preload-file ./BuiltIn --shell-file ${template_path}/Platforms/Emscripten/Shell/shell.html --disable-shared -s WASM=1 -s BINARYEN_METHOD='native-wasm'")
	
	set(project_name SamplePBR)
	configure_file(${template_path}/Platforms/Emscripten/Index.html ${SKYLICHT_ENGINE_BIN_DIR}/Index.html)
	configure_file(${template_path}/Platforms/Emscripten/Index.html ${SKYLICHT_ENGINE_BIN_DIR}/SamplePBR.html)
endif()

#VLD
if (BUILD_DEBUG_VLD)
	file(GLOB_RECURSE vld_bin_files ${vld_dll_path})
	foreach(vld_bin ${vld_bin_files})
		add_custom_command(TARGET SamplePBR POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${vld_bin} $<TARGET_FILE_DIR:SamplePBR>)
	endforeach()
endif()

#SDL
if (BUILD_SDL AND MSVC)
	file(GLOB_RECURSE sdl_bin_files ${sdl_dll_path})
	foreach(sdl_bin ${sdl_bin_files})
		add_custom_command(TARGET SamplePBR POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${sdl_bin} $<TARGET_FILE_DIR:SamplePBR>)
	endforeach()
endif()

set_target_properties(SamplePBR PROPERTIES VERSION ${SKYLICHT_VERSION})
set_target_properties(SamplePBR PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
