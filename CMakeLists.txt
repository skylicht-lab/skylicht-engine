cmake_minimum_required(VERSION 3.8.0)

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

# setup folder
set(SKYLICHT_ENGINE_BIN_DIR ${SKYLICHT_ENGINE_SOURCE_DIR}/Bin)
set(SKYLICHT_ENGINE_PROJECT_DIR ${SKYLICHT_ENGINE_SOURCE_DIR}/Projects)

# output
if (CMAKE_SYSTEM_NAME STREQUAL Android)
	set(LIBRARY_OUTPUT_PATH ${SKYLICHT_ENGINE_BIN_DIR}/${CMAKE_SYSTEM_NAME}/Libs/${ANDROID_ABI} CACHE PATH "Single output directory for building all libraries.")
elseif (CMAKE_SYSTEM_NAME STREQUAL iOS)
	set(LIBRARY_OUTPUT_PATH ${SKYLICHT_ENGINE_BIN_DIR}/${CMAKE_SYSTEM_NAME}/Libs/${PLATFORM} CACHE PATH "Single output directory for building all libraries.")
else()
	set(LIBRARY_OUTPUT_PATH ${SKYLICHT_ENGINE_BIN_DIR}/${CMAKE_SYSTEM_NAME}/Libs CACHE PATH "Single output directory for building all libraries.")
endif()

if (CMAKE_SYSTEM_NAME STREQUAL iOS)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SKYLICHT_ENGINE_BIN_DIR}/${PLATFORM} CACHE PATH "Single output directory for building executables.")
else()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SKYLICHT_ENGINE_BIN_DIR} CACHE PATH "Single output directory for building executables.")
endif()

# debug/release
if (NOT CMAKE_BUILD_TYPE)
	# set(CMAKE_BUILD_TYPE "Debug")
	set(CMAKE_BUILD_TYPE "Release")
endif()

message(STATUS "- System: ${CMAKE_SYSTEM_NAME} - BuildType: ${CMAKE_BUILD_TYPE}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
endif()

set(CMAKE_CXX_STANDARD 17)

option(BUILD_SHARED_LIBS "Use shared libraries" OFF)

option(INSTALL_LIBS "Install library" OFF)

option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC Runtime Library DLL (/MD or /MDd)" ON)

# build skylicht engine tool on Windows
option(BUILD_ENGINE_TOOL "Build with scene editor" ON)

# skylicht engine option
option(BUILD_DEBUG_VLD "Build with debug visual leak detector" OFF)

option(BUILD_SDL "Build with Simple DirectMedia Layer is a cross-platform development library" OFF)

option(USE_OPENMP "Use openmp for multithread optimize" ON)

include(SkylichtConfig.cmake)

include(PlatformConfig.cmake)

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
			
			find_library(OMP_LIBRARY NAMES omp)
			link_libraries(${OMP_LIBRARY})
			
			message(STATUS "- OpenMP library found: ${OMP_H_PATH}")
			message(STATUS "- OpenMP include directory: ${OMP_INCLUDE_DIR}")
			message(STATUS "- OpenMP link directory: ${OMP_LIBRARY}")
			
			add_definitions(-DUSE_OPENMP)
		else()
			message(STATUS "! OpenMP library not found! $>brew install libomp")
		endif()
	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		# using Visual Studio C++
		add_compile_options(/openmp)
		add_definitions(-DUSE_OPENMP)
	else()
		# using GCC & Clang++ (android)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
		add_definitions(-DUSE_OPENMP)
	endif()
endif()

# network
if (BUILD_SKYLICHT_NETWORK)
	add_definitions(-DBUILD_SKYLICHT_NETWORK)
	# enable curl
	set(BUILD_CURL ON)
	add_definitions(-DHAVE_CURL)
	add_definitions(-DCURL_STATICLIB)
	add_definitions(-DHTTP_ONLY)
	add_definitions(-DBUILDING_LIBCURL)
	# enable openssl for https
	if (MSVC)
	set(BUILD_OPENSSL ON)
	add_definitions(-DUSE_OPENSSL)
	endif()
else()
	set(BUILD_CURL OFF)
endif()

# freetype font library
if (BUILD_FREETYPE)
add_definitions(-DFT2_BUILD_LIBRARY)
endif()

# engine static library
if (NOT BUILD_SHARED_LIBS)
	add_definitions(-D_IRR_STATIC_LIB_)
	add_definitions(-D_SKYLICHT_STATIC_LIB_)
endif()

# skylicht debug leak memory
if (BUILD_DEBUG_VLD AND MSVC)
	add_definitions(-DUSE_VISUAL_LEAK_DETECTOR)
endif()

if (BUILD_SDL)
	add_definitions(-DSDL)
endif()

# bullet engine
if (BUILD_BULLET_PHYSIC_LIB)
	add_definitions(-DUSE_BULLET_PHYSIC_ENGINE)
endif()

# unicode library
add_definitions(-DUNICODE -D_UNICODE)

if (CYGWIN)
	add_definitions(-DCYGWIN)
	# gcc7 not support std::filesystem so we disable
	set(BUILD_ENGINE_TOOL OFF)
endif()

if (MINGW)
	add_definitions(-DMINGW)
	# gcc7 not support std::filesystem so we disable
	set(BUILD_ENGINE_TOOL OFF)
endif()

# android
if (BUILD_ANDROID)
	add_compile_options(-Wno-narrowing)
	add_compile_options(-fvisibility=hidden)
	add_compile_options(-O3)
endif()

# visual studio
if (BUILD_WINDOWS_STORE)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
		
	add_compile_options(/ZW)

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
		  add_compile_options(/fp:fast)
	endif()

	set(CMAKE_EXE_LINKER_FLAGS	"${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
endif()

# install output
set(CMAKE_INSTALL_PREFIX ${SKYLICHT_ENGINE_SOURCE_DIR}/InstallLibs)
set(SKYLICHT_LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
set(SKYLICHT_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include)

if (BUILD_SHARED_LIBS)
	message(STATUS "Building dynamic libs")
	set(ENGINE_SHARED_OR_STATIC_LIB "SHARED")
else()
	message(STATUS "Building static libs")
	set(ENGINE_SHARED_OR_STATIC_LIB "STATIC")
endif()

# add project

# core library
subdirs (Projects/ThirdParty)
subdirs (Projects/Irrlicht)

if (BUILD_IMGUI)
subdirs (Projects/Imgui)
endif()

# skylicht engine
subdirs (Projects/Skylicht/System)
subdirs (Projects/Skylicht/Engine)

# skylicht components
if (BUILD_SKYLICHT_COMPONENTS)
add_definitions(-DBUILD_SKYLICHT_COMPONENTS)
subdirs (Projects/Skylicht/Components)
endif()

# skylicht collision
if (BUILD_SKYLICHT_COLLISION)
add_definitions(-DBUILD_SKYLICHT_COLLISION)
subdirs (Projects/Skylicht/Collision)
endif()

# skylicht lightmapper
if (BUILD_SKYLICHT_LIGHMAPPER)
add_definitions(-DBUILD_SKYLICHT_LIGHMAPPER)
subdirs (Projects/Skylicht/Lightmapper)
endif()

# skylicht physics library
if (BUILD_SKYLICHT_PHYSIC)
add_definitions(-DBUILD_SKYLICHT_PHYSIC)
if (BUILD_BULLET_PHYSIC_LIB)
subdirs (Projects/Bullet3)
endif()
subdirs (Projects/Skylicht/Physics)
endif()

# skylicht audio
if (BUILD_SKYLICHT_AUDIO)
add_definitions(-DBUILD_SKYLICHT_AUDIO)
subdirs (Projects/Skylicht/Audio)
endif()

# skylicht ui
if (BUILD_SKYLICHT_UI)
add_definitions(-DBUILD_SKYLICHT_UI)
subdirs (Projects/Skylicht/UserInterface)
endif()

# skylicht crypto
if (BUILD_SKYLICHT_CRYPTO)
add_definitions(-DBUILD_SKYLICHT_CRYPTO)
subdirs (Projects/Skylicht/Crypto)
endif()

# skylicht network
if (BUILD_SKYLICHT_NETWORK)
subdirs (Projects/Skylicht/Network)
endif()

subdirs (Projects/Skylicht/Client)

if (NOT INSTALL_LIBS)

if (BUILD_ENGINE_TOOL)
subdirs (Projects/Editor)
endif()

include(CMakeProjects.cmake)

# unit test
if (NOT BUILD_ANDROID AND NOT BUILD_IOS AND NOT BUILD_EMSCRIPTEN AND NOT BUILD_WINDOWS_STORE)
enable_testing()
subdirs (UnitTest/TestApp)
endif()

endif()