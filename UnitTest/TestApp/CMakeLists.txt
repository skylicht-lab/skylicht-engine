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

if (BUILD_FREETYPE)
	include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/freetype2/include)
endif()

if (BUILD_IMGUI)
	include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Imgui/Source)
endif()

file(GLOB_RECURSE test_app_source 
	./**.cpp
	./**.c 
	./**.h)

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
	set(angle_lib_path "${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Angle/out/MacOS/Release")
	add_library(GLESv2 SHARED IMPORTED)
	set_target_properties(GLESv2 PROPERTIES IMPORTED_LOCATION "${angle_lib_path}/libGLESv2.dylib")
	target_link_libraries(TestApp GLESv2)

	add_custom_command(TARGET TestApp POST_BUILD COMMAND 
		${CMAKE_COMMAND} -E copy_if_different "${angle_lib_path}/libGLESv2.dylib" 
		$<TARGET_FILE_DIR:TestApp>)			
endif()

if (BUILD_MACOS)
	# WORKING_DIRECTORY for load dylib
	add_test(NAME TestApp COMMAND $<TARGET_FILE:TestApp> WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")
else()
	add_test(NAME TestApp COMMAND $<TARGET_FILE:TestApp>)
endif()