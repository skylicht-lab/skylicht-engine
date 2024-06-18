include_directories(
	${HELLO_SKYLICHT_SOURCE_DIR}/UnitTest/TestApp
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
)

add_definitions(-DTEST_APP)

if (BUILD_IMGUI)
	include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Imgui/Source)
endif()

set(template_path ${SKYLICHT_ENGINE_PROJECT_DIR}/Main)

if (BUILD_MACOS)
include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Angle/include)
include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Source/Angle)
include_directories(${template_path}/Platforms/MacOS)

file(GLOB_RECURSE test_app_source 
	./**.cpp
	./**.c 
	./**.h
	${template_path}/Platforms/MacOS/**.cpp 
	${template_path}/Platforms/MacOS/**.c
	${template_path}/Platforms/MacOS/**.h
	${template_path}/Platforms/MacOS/**.m
	${template_path}/Platforms/MacOS/**.mm)
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
target_link_libraries(TestApp Skylicht::Client)

if (BUILD_MACOS)
	set(angle_lib_path "${SKYLICHT_ENGINE_PROJECT_DIR}/Angle/out/MacOS/Release/${CMAKE_OSX_ARCHITECTURES}")
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