include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Template/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/System/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Engine/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Client/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Components/Source
)

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
endif()

add_test(NAME TestApp COMMAND $<TARGET_FILE:TestApp>)