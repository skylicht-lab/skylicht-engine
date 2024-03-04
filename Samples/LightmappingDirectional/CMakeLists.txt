include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Samples/LightmappingDirectional/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Samples/SampleFW
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Main/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/System/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Engine/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Components/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Collision/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Physics/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Client/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Lightmapper/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Audio/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/freetype2/include
)

if (BUILD_IMGUI)
	include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Imgui/Source)
endif()

set(template_path ${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Main)

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
	file(MAKE_DIRECTORY ${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/LightmappingDirectional)
	file(MAKE_DIRECTORY ${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/LightmappingDirectional/Assets)
	
	file(GLOB uwp_assets 
		${template_path}/Platforms/UWP/Assets/**.*
		${SKYLICHT_ENGINE_SOURCE_DIR}/Bin/**.zip)
	
	foreach(asset_file ${uwp_assets})
		file(COPY "${asset_file}" DESTINATION ${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/LightmappingDirectional/Assets)
	endforeach()

	file(COPY ${template_path}/Platforms/UWP/Package.appxmanifest
		DESTINATION 
		${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/LightmappingDirectional)
		
	file(COPY ${template_path}/Platforms/UWP/TemporaryKey.pfx
		DESTINATION 
		${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/LightmappingDirectional)

	file(GLOB_RECURSE platform_uwp_source 
		${template_path}/Platforms/UWP/**.cpp 
		${template_path}/Platforms/UWP/**.c 
		${template_path}/Platforms/UWP/**.h)
	
	# copy resource to generate prj folder
	file(GLOB_RECURSE platform_uwp_asset ${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/LightmappingDirectional/Assets/**.*)
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
	include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Angle/include)
	include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple/Angle)
	include_directories(${template_path}/Platforms/MacOS)
	
	file(GLOB_RECURSE platform_mac_source 
		${template_path}/Platforms/MacOS/**.cpp 
		${template_path}/Platforms/MacOS/**.c 
		${template_path}/Platforms/MacOS/**.h
		${template_path}/Platforms/MacOS/**.m
		${template_path}/Platforms/MacOS/**.mm
		${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple/*.h
		${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple/*.m
		${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple/*.mm
		${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple/*.cpp
		${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple/*.c)

	list (APPEND application_source ${platform_mac_source})

	setup_project_group_by_folder_relative("${platform_mac_source}" "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple")
endif()

if (BUILD_IOS)
	# Angle API
	include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Angle/include)
	include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple/Angle)
	include_directories(${template_path}/Platforms/IOS)
	
	file(GLOB_RECURSE platform_ios_source 
		${template_path}/Platforms/IOS/**.cpp 
		${template_path}/Platforms/IOS/**.c 
		${template_path}/Platforms/IOS/**.h
		${template_path}/Platforms/IOS/**.m
		${template_path}/Platforms/IOS/**.mm
		${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple/*.h
		${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple/*.m
		${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple/*.mm
		${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple/*.cpp
		${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple/*.c)

	file(GLOB_RECURSE platform_ios_resources ${template_path}/Platforms/IOS/**.storyboard)

	list (APPEND application_source ${platform_ios_source})

	setup_project_group_by_folder_relative("${platform_ios_source}" "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple")
endif()

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
	
if (BUILD_SDL AND MSVC)
	if (CMAKE_CL_64)
		set(sdl_lib_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdPartySDK/SDL/lib/x64")
		set(sdl_dll_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdPartySDK/SDL/lib/x64/*.dll")
	else()
		set(sdl_lib_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdPartySDK/SDL/lib/x86")
		set(sdl_dll_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdPartySDK/SDL/lib/x86/*.dll")
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
	add_executable(SampleLightmappingDirectional 
		${application_source}
		${template_path}/Platforms/Emscripten/MainWebGL.cpp
	)
elseif(BUILD_SDL)
	add_executable(SampleLightmappingDirectional 
		${application_source}
		${template_path}/Platforms/SDL2/MainSDL.cpp
	)
elseif(BUILD_LINUX)
	add_executable(SampleLightmappingDirectional 
		${application_source}
		${template_path}/Platforms/Linux/MainLinux.cpp
	)
elseif(BUILD_ANDROID)
	add_library(SampleLightmappingDirectional SHARED ${application_source})
elseif (BUILD_WINDOWS_STORE)
	add_executable(SampleLightmappingDirectional
		${application_source}
		${platform_uwp_asset}
		${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/LightmappingDirectional/TemporaryKey.pfx
		${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/LightmappingDirectional/Package.appxmanifest
	)
elseif (MSVC OR CYGWIN OR MINGW)
	add_executable(SampleLightmappingDirectional WIN32
		${application_source}
		${template_path}/Platforms/Win32/Skylicht.rc
	)
elseif (BUILD_MACOS)
	file(GLOB resources_files "${SKYLICHT_ENGINE_SOURCE_DIR}/Bin/*.zip")

	set(angle_lib_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Angle/out/MacOS/Release/${CMAKE_OSX_ARCHITECTURES}")
	file(GLOB dylib_files "${angle_lib_path}/*.dylib")

	add_executable(SampleLightmappingDirectional 
		${application_source} 
		${resources_files}
		${dylib_files}
	)
	
	set_target_properties(SampleLightmappingDirectional PROPERTIES MACOSX_BUNDLE TRUE)
	
	set(angle_lib_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Angle/out/MacOS/Release/${CMAKE_OSX_ARCHITECTURES}")
	
	target_link_libraries(SampleLightmappingDirectional "-framework Cocoa")
		
	message(STATUS "- Setup project: SampleLightmappingDirectional")
	foreach(res_file ${resources_files})
		file(RELATIVE_PATH res_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Bin" ${res_file})
		message(STATUS "  - Add resources: ${res_path}")
		set_property(SOURCE ${res_file} PROPERTY MACOSX_PACKAGE_LOCATION "Resources")
		source_group("Bin" FILES "${res_file}")
	endforeach()

	foreach(lib_file ${dylib_files})
		set_property(SOURCE ${lib_file} PROPERTY MACOSX_PACKAGE_LOCATION "Frameworks")
		set_source_files_properties(${lib_file} PROPERTIES XCODE_FILE_ATTRIBUTES "CodeSignOnCopy")
		source_group("Libs" FILES "${lib_file}")
	endforeach()

	add_custom_command(TARGET SampleLightmappingDirectional 
		POST_BUILD COMMAND 
		${CMAKE_INSTALL_NAME_TOOL} -change ./libEGL.dylib @rpath/libEGL.dylib
		$<TARGET_FILE:SampleLightmappingDirectional>)

	add_custom_command(TARGET SampleLightmappingDirectional 
		POST_BUILD COMMAND 
		${CMAKE_INSTALL_NAME_TOOL} -change ./libGLESv2.dylib @rpath/libGLESv2.dylib
		$<TARGET_FILE:SampleLightmappingDirectional>)

	set_target_properties(SampleLightmappingDirectional PROPERTIES
		MACOSX_RPATH ON
		BUILD_WITH_INSTALL_RPATH 1
		INSTALL_RPATH "@executable_path/../Frameworks"
	)
elseif (BUILD_IOS)
	set(MACOSX_BUNDLE_EXECUTABLE_NAME SampleLightmappingDirectional)

	set(APP_NAME "SampleLightmappingDirectional")
	set(APP_BUNDLE_IDENTIFIER "com.skylicht.samplelightmappingdirectional")
	set(CODE_SIGN_IDENTITY "iPhone Developer")

	set(PRODUCT_NAME ${APP_NAME})
	set(EXECUTABLE_NAME ${APP_NAME})
	set(MACOSX_BUNDLE_EXECUTABLE_NAME ${APP_NAME})
	set(MACOSX_BUNDLE_BUNDLE_NAME ${APP_BUNDLE_IDENTIFIER})
	set(MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_BUNDLE_IDENTIFIER})

	file(GLOB resources_files "${SKYLICHT_ENGINE_SOURCE_DIR}/Bin/*.zip")

	set(angle_lib_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Angle/out/IOS/Release/${IOS_PLATFORM}")
	file(GLOB angle_framework_files "${angle_lib_path}/*.framework")

	set (framework_files "")
	list (APPEND framework_files ${angle_framework_files})

	add_executable(SampleLightmappingDirectional MACOSX_BUNDLE 
		${application_source}
		${resources_files}
		${platform_ios_resources}
		${framework_files}
	)
	
	message(STATUS "- Setup project: SampleLightmappingDirectional")
	foreach(res_file ${resources_files})
		file(RELATIVE_PATH res_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Bin" ${res_file})
		source_group("Bin" FILES "${res_file}")
	endforeach()

	foreach(lib_file ${framework_files})
		source_group("Frameworks" FILES "${lib_file}")
		set_property(SOURCE ${lib_file} PROPERTY MACOSX_PACKAGE_LOCATION "Frameworks")
		set_source_files_properties(${lib_file} PROPERTIES XCODE_FILE_ATTRIBUTES "CodeSignOnCopy")
	endforeach()

	set_target_properties(SampleLightmappingDirectional PROPERTIES XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER ${APP_BUNDLE_IDENTIFIER})
	set_target_properties(SampleLightmappingDirectional PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ${CODE_SIGN_IDENTITY})
	set_target_properties(SampleLightmappingDirectional PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY ${DEVICE_FAMILY})
	set_target_properties(SampleLightmappingDirectional PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${template_path}/Platforms/IOS/plist.in")
	
	# add assets icon
	target_sources(SampleLightmappingDirectional PRIVATE "${template_path}/Platforms/IOS/Assets.xcassets")

	list (APPEND platform_ios_resources "${template_path}/Platforms/IOS/Assets.xcassets")
	set_target_properties(SampleLightmappingDirectional PROPERTIES RESOURCE "${resources_files} ${platform_ios_resources}")

	# link framework
	target_link_libraries(SampleLightmappingDirectional "-framework Foundation")
	target_link_libraries(SampleLightmappingDirectional "-framework UIKit")
	target_link_libraries(SampleLightmappingDirectional "-framework CoreGraphics")
	target_link_libraries(SampleLightmappingDirectional "-framework Metal")
	target_link_libraries(SampleLightmappingDirectional "-framework MetalKit")
	target_link_libraries(SampleLightmappingDirectional "-framework AudioToolbox")
	target_link_libraries(SampleLightmappingDirectional "-framework AVFAudio")
else()
	add_executable(SampleLightmappingDirectional ${application_source})
endif()

target_precompiled_header(SampleLightmappingDirectional ${template_path}/Source/pch.cpp ${application_source})

# Linker
if (BUILD_ANDROID)
	target_link_libraries(SampleLightmappingDirectional Client log)
else()
	target_link_libraries(SampleLightmappingDirectional Client)
endif()

# Imgui
if (BUILD_IMGUI)
	target_link_libraries(SampleLightmappingDirectional Imgui)
endif()

# Emscripten
if (BUILD_EMSCRIPTEN)
	message(STATUS "Setting build in data: ${CMAKE_CURRENT_BINARY_DIR}")
	file(COPY "${SKYLICHT_ENGINE_SOURCE_DIR}/Bin/BuiltIn" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
	
	message(STATUS "Setting compilation target to WASM")
	set(CMAKE_EXECUTABLE_SUFFIX ".wasm.html")
	set_target_properties(SampleLightmappingDirectional PROPERTIES LINK_FLAGS "-s USE_SDL=2 -s USE_WEBGL2=1 -s FORCE_FILESYSTEM=1 -s FETCH=1 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_FUNCTIONS=['_main','_main_resize'] -s EXPORTED_RUNTIME_METHODS=['ccall'] --preload-file ./BuiltIn --shell-file ${template_path}/Platforms/Emscripten/Shell/shell.html --disable-shared -s WASM=1 -s BINARYEN_METHOD='native-wasm'")
	
	set(project_name SampleLightmappingDirectional)
	configure_file(${template_path}/Platforms/Emscripten/Index.html ${SKYLICHT_ENGINE_SOURCE_DIR}/Bin/Index.html)
	configure_file(${template_path}/Platforms/Emscripten/Index.html ${SKYLICHT_ENGINE_SOURCE_DIR}/Bin/SampleLightmappingDirectional.html)
endif()

#VLD
if (BUILD_DEBUG_VLD)
	file(GLOB_RECURSE vld_bin_files ${vld_dll_path})
	foreach(vld_bin ${vld_bin_files})
		add_custom_command(TARGET SampleLightmappingDirectional POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${vld_bin} $<TARGET_FILE_DIR:SampleLightmappingDirectional>)
	endforeach()
endif()

#SDL
if (BUILD_SDL AND MSVC)
	file(GLOB_RECURSE sdl_bin_files ${sdl_dll_path})
	foreach(sdl_bin ${sdl_bin_files})
		add_custom_command(TARGET SampleLightmappingDirectional POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${sdl_bin} $<TARGET_FILE_DIR:SampleLightmappingDirectional>)
	endforeach()
endif()

set_target_properties(SampleLightmappingDirectional PROPERTIES VERSION ${SKYLICHT_VERSION})
set_target_properties(SampleLightmappingDirectional PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
