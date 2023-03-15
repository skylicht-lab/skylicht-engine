include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/UnitTest/TestApp
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Template/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include	
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/System/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Engine/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Components/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Client/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Lightmapper/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Audio/Source
	#${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Physics/Source
)

add_definitions(-DTEST_APP)

if (BUILD_FREETYPE)
	include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/freetype2/include)
endif()

if (BUILD_IMGUI)
	include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Imgui/Source)
endif()

set(template_path ${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Template)

if (BUILD_MACOS)
include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Angle/include)
include_directories(${template_path}/Platforms/MacOS)

file(GLOB_RECURSE test_app_source 
	./**.cpp
	./**.c 
	./**.h
	${template_path}/Platforms/MacOS/**.cpp 
	${template_path}/Platforms/MacOS/**.c
	${template_path}/Platforms/MacOS/**.h
	${template_path}/Platforms/MacOS/**.m
	${template_path}/Platforms/MacOS/**.mm
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple/*.h
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple/*.m
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Apple/*.mm)
else()
file(GLOB_RECURSE test_app_source 
	./**.cpp
	./**.c 
	./**.h)
endif()

if (MINGW OR CYGWIN)
	add_executable(TestApp WIN32 ${test_app_source})
else()
	add_executable(TestApp ${test_app_source})
endif()

# Linker
if (BUILD_ANDROID)
	target_link_libraries(TestApp Client log)
else()
	target_link_libraries(TestApp Client)
endif()

if (BUILD_MACOS)
	set(angle_lib_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Angle/out/MacOS/Release/${CMAKE_OSX_ARCHITECTURES}")

	find_library(
		LIBEGL 
		libEGL.dylib
		PATHS ${angle_lib_path})

	find_library(
		LIBGLESV2
		libGLESv2.dylib
		PATHS ${angle_lib_path})

	target_link_libraries(TestApp ${LIBEGL} ${LIBGLESV2})
	target_link_libraries(TestApp "-framework Cocoa")

	add_custom_command(TARGET TestApp POST_BUILD COMMAND 
		${CMAKE_COMMAND} -E copy_if_different "${angle_lib_path}/libGLESv2.dylib" 
		$<TARGET_FILE_DIR:TestApp>)

	add_custom_command(TARGET TestApp POST_BUILD COMMAND 
		${CMAKE_COMMAND} -E copy_if_different "${angle_lib_path}/libEGL.dylib" 
		$<TARGET_FILE_DIR:TestApp>)	
endif()

if (BUILD_MACOS)
	# WORKING_DIRECTORY for load dylib
	add_test(NAME TestApp COMMAND $<TARGET_FILE:TestApp> WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")
else()
	add_test(NAME TestApp COMMAND $<TARGET_FILE:TestApp>)
endif()

set_target_properties(TestApp PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")