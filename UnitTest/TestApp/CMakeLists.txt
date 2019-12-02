include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/MainApp/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtSystem/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtEngine/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/SkylichtClient/Source
)

file(GLOB_RECURSE test_app_source 
	./**.cpp
	./**.hpp
	./**.c 
	./**.h)

if (MINGW OR CYGWIN)
	add_executable(TestApp WIN32 ${test_app_source})
else()
	add_executable(TestApp ${test_app_source})
endif()

# Linker
if (BUILD_ANDROID)
	target_link_libraries(TestApp SkylichtClient log)
else()
	target_link_libraries(TestApp SkylichtClient)
endif()

add_test(NAME TestApp COMMAND $<TARGET_FILE:TestApp>)