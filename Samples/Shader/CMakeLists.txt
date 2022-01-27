include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Samples/Shader/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Template/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include	
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/System/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Engine/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Components/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Collision/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Physics/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Client/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Lightmapper/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Audio/Source
)

if (BUILD_FREETYPE)
	include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/freetype2/include)
endif()

if (BUILD_IMGUI)
	include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Imgui/Source)
endif()

set(template_path ${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Template)

file(GLOB_RECURSE application_source 
	./Source/**.cpp 
	./Source/**.c 
	./Source/**.h)

setup_project_group("${application_source}" ${CMAKE_CURRENT_SOURCE_DIR})

list (APPEND application_source ${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Template/Source/pch.cpp)
list (APPEND application_source ${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Template/Source/pch.h)

if (BUILD_ANDROID)
	file(GLOB_RECURSE platform_android_source 
		${template_path}/Platforms/Android/**.cpp 
		${template_path}/Platforms/Android/**.c 
		${template_path}/Platforms/Android/**.h)

	list (APPEND application_source ${platform_android_source})
endif()

if (BUILD_WINDOWS_STORE)
	file(MAKE_DIRECTORY ${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/Shader)
	file(MAKE_DIRECTORY ${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/Shader/Assets)
	
	file(GLOB uwp_assets 
		${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Template/Platforms/UWP/Assets/**.*
		${SKYLICHT_ENGINE_SOURCE_DIR}/Bin/**.zip)
	
	foreach(asset_file ${uwp_assets})
		file(COPY "${asset_file}" DESTINATION ${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/Shader/Assets)
	endforeach()

	file(COPY ${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Template/Platforms/UWP/Package.appxmanifest
		DESTINATION 
		${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/Shader)
		
	file(COPY ${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Template/Platforms/UWP/TemporaryKey.pfx
		DESTINATION 
		${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/Shader)

	file(GLOB_RECURSE platform_uwp_source 
		${template_path}/Platforms/UWP/**.cpp 
		${template_path}/Platforms/UWP/**.c 
		${template_path}/Platforms/UWP/**.h)
	
	# copy resource to generate prj folder
	file(GLOB_RECURSE platform_uwp_asset ${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/Shader/Assets/**.*)
	add_source_group("${platform_uwp_asset}" "Assets")
	
	list (APPEND application_source ${platform_uwp_source})

	set_property(SOURCE ${platform_uwp_asset} PROPERTY VS_DEPLOYMENT_CONTENT 1)
endif()

if (MSVC OR CYGWIN OR MINGW)
	include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Template/Platforms/Win32)

	file(GLOB_RECURSE platform_win32_source 
		${template_path}/Platforms/Win32/**.cpp 
		${template_path}/Platforms/Win32/**.c 
		${template_path}/Platforms/Win32/**.h)

	list (APPEND application_source ${platform_win32_source})
endif()

if (BUILD_MACOS)
	# Angle API
	include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Angle/include)
	include_directories(${template_path}/Platforms/MacOS)
	
	file(GLOB_RECURSE platform_mac_source 
		${template_path}/Platforms/MacOS/**.cpp 
		${template_path}/Platforms/MacOS/**.c 
		${template_path}/Platforms/MacOS/**.h
		${template_path}/Platforms/MacOS/**.m
		${template_path}/Platforms/MacOS/**.mm
		${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple/*.h
		${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple/*.m
		${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple/*.mm)

	list (APPEND application_source ${platform_mac_source})

	setup_project_group("${platform_mac_source}" ${CMAKE_CURRENT_SOURCE_DIR})
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
	add_executable(SampleShader 
		${application_source}
		${template_path}/Platforms/Emscripten/MainWebGL.cpp
	)
elseif(BUILD_SDL)
	add_executable(SampleShader 
		${application_source}
		${template_path}/Platforms/SDL2/MainSDL.cpp
	)
elseif(BUILD_LINUX)
	add_executable(SampleShader
		${application_source}
		${template_path}/Platforms/Linux/MainLinux.cpp
	)
elseif(BUILD_ANDROID)
	add_library(SampleShader SHARED ${application_source})
elseif (BUILD_WINDOWS_STORE)
	add_executable(SampleShader
		${application_source}
		${platform_uwp_asset}
		${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/Shader/TemporaryKey.pfx
		${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/Shader/Package.appxmanifest
	)
elseif (MSVC OR CYGWIN OR MINGW)
	add_executable(SampleShader WIN32
		${application_source}
		${template_path}/Platforms/Win32/Skylicht.rc
	)
elseif (BUILD_MACOS)
	file(GLOB resources_files "${SKYLICHT_ENGINE_SOURCE_DIR}/Bin/*.zip")

	set(angle_lib_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Angle/out/MacOS/Release")
	file(GLOB dylib_files "${angle_lib_path}/*.dylib")

	add_executable(SampleShader 
		${application_source} 
		${resources_files}
		${dylib_files}
	)
	
	set_target_properties(SampleShader PROPERTIES MACOSX_BUNDLE TRUE)
	
	set(angle_lib_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Angle/out/MacOS/Release")
	
	find_library(
		LIBEGL 
		libEGL.dylib
		PATHS ${angle_lib_path})

	find_library(
		LIBGLESV2
		libGLESv2.dylib
		PATHS ${angle_lib_path})

	target_link_libraries(SampleShader ${LIBEGL} ${LIBGLESV2})
	target_link_libraries(SampleShader "-framework Cocoa")
		
	message(STATUS "- Setup project: SampleShader")
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

	add_custom_command(TARGET SampleShader 
		POST_BUILD COMMAND 
		${CMAKE_INSTALL_NAME_TOOL} -change ./libEGL.dylib @rpath/libEGL.dylib
		$<TARGET_FILE:SampleShader>)

	add_custom_command(TARGET SampleShader 
		POST_BUILD COMMAND 
		${CMAKE_INSTALL_NAME_TOOL} -change ./libGLESv2.dylib @rpath/libGLESv2.dylib
		$<TARGET_FILE:SampleShader>)

	set_target_properties(SampleShader PROPERTIES
		MACOSX_RPATH ON
  		BUILD_WITH_INSTALL_RPATH 1
		INSTALL_RPATH "@executable_path/../Frameworks"
	)
else()
	add_executable(SampleShader ${application_source})
endif()

target_precompiled_header(SampleShader ${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Template/Source/pch.cpp ${application_source})

# Linker
if (BUILD_ANDROID)
	target_link_libraries(SampleShader Client log)
else()
	target_link_libraries(SampleShader Client)
endif()

# Imgui
if (BUILD_IMGUI)
	target_link_libraries(SampleShader Imgui)
endif()

# Emscripten
if (BUILD_EMSCRIPTEN)
	message(STATUS "Setting build in data: ${CMAKE_CURRENT_BINARY_DIR}")
	file(COPY "${SKYLICHT_ENGINE_SOURCE_DIR}/Bin/BuiltIn" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
	
	message(STATUS "Setting compilation target to WASM")
	set(CMAKE_EXECUTABLE_SUFFIX ".wasm.html")
	set_target_properties(SampleShader PROPERTIES LINK_FLAGS "-s USE_SDL=2 -s USE_WEBGL2=1 -s FORCE_FILESYSTEM=1 -s FETCH=1 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_FUNCTIONS=['_main','_main_resize'] -s EXPORTED_RUNTIME_METHODS=['ccall'] --preload-file ./BuiltIn --shell-file ${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Template/Platforms/Emscripten/Shell/shell.html --memory-init-file 1 --disable-shared -s WASM=1 -s BINARYEN_METHOD='native-wasm'")
	
	set(project_name SampleShader)
	configure_file(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Template/Platforms/Emscripten/Index.html ${SKYLICHT_ENGINE_SOURCE_DIR}/Bin/Index.html)
	configure_file(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Template/Platforms/Emscripten/Index.html ${SKYLICHT_ENGINE_SOURCE_DIR}/Bin/SampleShader.html)
endif()

#VLD
if (BUILD_DEBUG_VLD)
	file(GLOB_RECURSE vld_bin_files ${vld_dll_path})
	foreach(vld_bin ${vld_bin_files})
		add_custom_command(TARGET SampleShader POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${vld_bin} $<TARGET_FILE_DIR:SampleShader>)
	endforeach()
endif()

set_target_properties(SampleShader PROPERTIES VERSION ${SKYLICHT_VERSION})
set_target_properties(SampleShader PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
