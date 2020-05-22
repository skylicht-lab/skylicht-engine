cmake_minimum_required(VERSION 3.1)

# cmake config
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

# cmake library
include(CMake/CMakeVisualStudioSourceGroup.cmake)
include(CMake/CMakeVisualStudioPCH.cmake)

# this line has to appear before 'PROJECT' in order to be able to disable incremental linking
set(MSVC_INCREMENTAL_DEFAULT ON)

# begin project
project(SKYLICHT_ENGINE)
file(STRINGS "Version.txt" SKYLICHT_VERSION)
message(STATUS "- Skylicht Engine version ${SKYLICHT_VERSION}")

# output
set(LIBRARY_OUTPUT_PATH ${SKYLICHT_ENGINE_SOURCE_DIR}/Bin/${CMAKE_SYSTEM_NAME}/Libs CACHE PATH "Single output directory for building all libraries.")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SKYLICHT_ENGINE_SOURCE_DIR}/Bin CACHE PATH "Single output directory for building executables.")

# debug/release
if (NOT CMAKE_BUILD_TYPE)
	# set(CMAKE_BUILD_TYPE "Debug")
	set(CMAKE_BUILD_TYPE "Release")
endif()

message(STATUS "- System: ${CMAKE_SYSTEM_NAME} - BuildType: ${CMAKE_BUILD_TYPE}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
endif()

set(CMAKE_CXX_STANDARD 11)

option(BUILD_SHARED_LIBS "Use shared libraries" OFF)

option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC Runtime Library DLL (/MD or /MDd)" ON)

option(USE_OPENMP "Use openmp for multithread optimize" ON)

# build skylicht engine tool on Windows
set(BUILD_ENGINE_TOOL ON)
set(BUILD_CURL OFF)

if (CMAKE_SYSTEM_NAME STREQUAL WindowsStore)
	set(BUILD_ENGINE_TOOL OFF)
	set(BUILD_WINDOWS_STORE ON)
	add_definitions(-DWINDOWS_STORE)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
	set(BUILD_ENGINE_TOOL OFF)
	set(BUILD_EMSCRIPTEN ON)
	set(USE_OPENMP OFF)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL Android)
	set(BUILD_ENGINE_TOOL OFF)
	set(BUILD_ANDROID ON)
	add_definitions(-DANDROID)	
endif()

if (CMAKE_SYSTEM_NAME STREQUAL Linux)
	set(BUILD_ENGINE_TOOL OFF)
	set(BUILD_LINUX ON)
	add_definitions(-DLINUX)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL Darwin)
	set(BUILD_ENGINE_TOOL OFF)
	set(BUILD_MACOS ON)
	set(XCODE ON)
	add_definitions(-DMACOS)

	set(CMAKE_CXX_STANDARD 14)
	set(CMAKE_OSX_ARCHITECTURES "x86_64")

	# Angle API defined
	add_definitions(-DANGLE_USE_UTIL_LOADER)
	add_definitions(-DGL_GLES_PROTOTYPES=0)
	add_definitions(-DEGL_EGL_PROTOTYPES=0)
	add_definitions(-DANGLE_EGL_LIBRARY_NAME=\"libEGL\")
	add_definitions(-DANGLE_GLESV2_LIBRARY_NAME=\"libGLESv2\")
endif()

# irrlicht static lib
if (NOT BUILD_SHARED_LIBS)
	add_definitions(-D_IRR_STATIC_LIB_ )
endif()

# skylicht engine option
option(BUILD_SKYLICHT_NETWORK "Build Skylicht Network" ON)

option(BUILD_DEBUG_VLD "Build with debug visual leak detector" OFF)

option(BUILD_SDL "Build with Simple DirectMedia Layer is a cross-platform development library" OFF)

option(BUILD_IMGUI "Build Ocornut Imgui" ON)

option(BUILD_FREETYPE "Build Font Freetype" ON)

if (BUILD_FREETYPE)
	add_definitions(-DUSE_FREETYPE)
	add_definitions(-DFT2_BUILD_LIBRARY)
endif()

# skylicht network
if (BUILD_SKYLICHT_NETWORK)
	add_definitions(-DUSE_SKYLICHT_NETWORK)
endif()

# skylicht debug leak memory
if (BUILD_DEBUG_VLD AND MSVC)
	add_definitions(-DUSE_VISUAL_LEAK_DETECTOR)
endif()

if (BUILD_SDL)
	add_definitions(-DSDL)
	set(BUILD_ENGINE_TOOL OFF)
endif()

# skylicht bullet engine
option(BUILD_BULLET_PHYSIC2_LIB "Build bullet physic 2" ON)

if (BUILD_BULLET_PHYSIC2_LIB)
	add_definitions(-DUSE_PHYSIC_ENGINE -DUSE_PHYSIC_RAGDOLL -DUSE_PHYSIC_VEHICLE -DUSE_BULLET_PHYSIC_ENGINE)
	
	# sse on window
	if (MSVC OR CYGWIN OR MINGW)
		add_definitions(-DBT_USE_SSE)
	endif()
	
	if (MINGW)
		add_definitions(-DBT_NO_SIMD_OPERATOR_OVERLOADS)
		set(CMAKE_C_FLAGS " -mssse3 ${CMAKE_C_FLAGS}")
		set(CMAKE_CXX_FLAGS " -mssse3 ${CMAKE_CXX_FLAGS}")
	endif()
endif()

# skylicht recast navigation
option(BUILD_RECAST_NAVIGATION_LIB "Build Recast Navigation" ON)

if (BUILD_RECAST_NAVIGATION_LIB)
	add_definitions(-DUSE_RECAST_NAVIGATION_ENGINE)
endif()

# unicode library
add_definitions(-DUNICODE -D_UNICODE)

if (CYGWIN)
	add_definitions(-DCYGWIN)
endif()

if (MINGW)
	add_definitions(-DMINGW)
endif()

if (USE_OPENMP)
	if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
		# AppleClang
		find_package(OpenMP REQUIRED)
		if (OPENMP_FOUND)
			set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp")
			set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
			
			set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
			set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
			
			find_file(OMP_H_PATH NAME omp.h HINTS OpenMP)
			get_filename_component(OMP_INCLUDE_DIR ${OMP_H_PATH} DIRECTORY)
			include_directories(${OMP_INCLUDE_DIR})
			
			find_library(OMP_LIBRARY NAME omp)
			link_directories(${OMP_LIBRARY})
			
			message(STATUS "- OpenMP library found: ${OMP_H_PATH}")
			message(STATUS "- OpenMP include directory: ${OMP_INCLUDE_DIR}")
			message(STATUS "- OpenMP link directory: ${OMP_LIBRARY}")
			
			add_definitions(-DUSE_OPENMP)
		else()
			message(STATUS "! OpenMP library not found! $>brew install libomp")
		endif()
	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		# using Visual Studio C++
		set(CMAKE_C_FLAGS "/openmp ${CMAKE_C_FLAGS}")
		set(CMAKE_CXX_FLAGS "/openmp ${CMAKE_CXX_FLAGS}")
		add_definitions(-DUSE_OPENMP)
	else()
		# using GCC & Clang++ (android)
		set(CMAKE_CXX_FLAGS "-fopenmp ${CMAKE_CXX_FLAGS}")
		set(CMAKE_C_FLAGS "-fopenmp ${CMAKE_C_FLAGS}")
		add_definitions(-DUSE_OPENMP)
	endif()
endif()

# android
if (BUILD_ANDROID)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-narrowing")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-narrowing")
endif()

# visual studio
if (BUILD_WINDOWS_STORE)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)

	set(CMAKE_C_FLAGS "/ZW ${CMAKE_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "/ZW ${CMAKE_CXX_FLAGS}")
	
	if (CMAKE_CL_64)
		add_definitions(-D_WIN64)
	endif()
elseif (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)

	if (NOT USE_MSVC_RUNTIME_LIBRARY_DLL)
		#We statically link to reduce dependancies
		foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
			if (${flag_var} MATCHES "/MD")
				string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
			endif (${flag_var} MATCHES "/MD")
			if (${flag_var} MATCHES "/MDd")
				string(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
			endif (${flag_var} MATCHES "/MDd")
		endforeach()
	endif()	
	
	if (CMAKE_CL_64)
		add_definitions(-D_WIN64)
	endif()
	
	option(USE_MSVC_FAST_FLOATINGPOINT "Use MSVC /fp:fast option" ON)
	if (USE_MSVC_FAST_FLOATINGPOINT)
		  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast")
	endif()

	option(USE_MSVC_STRING_POOLING "Use MSVC /GF string pooling option"	ON)
	if (USE_MSVC_STRING_POOLING)
		set(CMAKE_C_FLAGS "/GF ${CMAKE_C_FLAGS}")
		set(CMAKE_CXX_FLAGS "/GF ${CMAKE_CXX_FLAGS}")
	endif()

	option(USE_MSVC_FUNCTION_LEVEL_LINKING "Use MSVC /Gy function level linking option"	ON)
	if (USE_MSVC_FUNCTION_LEVEL_LINKING)
		set(CMAKE_C_FLAGS "/Gy ${CMAKE_C_FLAGS}")
		set(CMAKE_CXX_FLAGS "/Gy ${CMAKE_CXX_FLAGS}")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /OPT:REF")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /OPT:REF")
	endif()
	
	option(USE_MSVC_COMDAT_FOLDING "Use MSVC /OPT:ICF COMDAT folding option" ON)
	if (USE_MSVC_COMDAT_FOLDING)
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /OPT:ICF")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /OPT:ICF")
	endif()

	set(CMAKE_EXE_LINKER_FLAGS	"${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
endif()

# add project

# core library
subdirs (Projects/ThirdParty)
subdirs (Projects/Irrlicht)
#subdirs (Projects/Spark)

if (BUILD_IMGUI)
subdirs (Projects/Imgui)
endif()

#if (BUILD_BULLET_PHYSIC2_LIB)
#subdirs (Projects/BulletPhysics2)
#endif()

#if (BUILD_RECAST_NAVIGATION_LIB)
#subdirs (Projects/RecastNavigation)
#endif()

# skylicht engine
subdirs (Projects/Skylicht/System)
subdirs (Projects/Skylicht/Engine)
subdirs (Projects/Skylicht/Components)
subdirs (Projects/Skylicht/Lightmapper)
#subdirs (Projects/Skylicht/Audio)
#subdirs (Projects/Skylicht/Physics)

#if (BUILD_SKYLICHT_NETWORK)
#subdirs (Projects/Skylicht/Network)
#endif()

subdirs (Projects/Skylicht/Client)

#if (BUILD_ENGINE_TOOL)
#subdirs (Projects/Editor)
#endif()

include(CMakeProjects.cmake)

# unit test
if (NOT BUILD_ANDROID AND NOT BUILD_EMSCRIPTEN AND NOT BUILD_WINDOWS_STORE)
enable_testing()
subdirs (UnitTest/TestApp)
endif()