option(USE_OPENMP "Use openmp for multithread optimize" ON)

if (CMAKE_SYSTEM_NAME STREQUAL WindowsStore)
	set(BUILD_ENGINE_TOOL OFF)
	set(BUILD_WINDOWS_STORE ON)
	add_definitions(-DWINDOWS_STORE)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
	set(BUILD_ENGINE_TOOL OFF)
	set(BUILD_EMSCRIPTEN ON)
	set(USE_OPENMP OFF)
	set(USE_SKYLICHT_NETWORK OFF)
	set(BUILD_SKYLICHT_NETWORK OFF)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL Android)
	set(BUILD_ENGINE_TOOL OFF)
	set(BUILD_ANDROID ON)
	set(USE_OPENMP OFF)
	add_definitions(-DANDROID)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL Linux)
	set(BUILD_LINUX ON)
	add_definitions(-DLINUX)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL Darwin OR CMAKE_SYSTEM_NAME STREQUAL iOS)
	if (IOS)
		# IOS
		set(IOS_PLATFORM "${PLATFORM}")
		set(BUILD_IOS ON)
		set(BUILD_ENGINE_TOOL OFF)
		add_definitions(-DIOS)
		
		# default for all sample
		set(DEVICE_FAMILY "1,2")
		set(MACOSX_BUNDLE_COPYRIGHT "Copyright Skylicht LTD.Co")
		set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")
		set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")

		# add flag for simulator
		if (IOS_PLATFORM STREQUAL "SIMULATORARM64")
			add_definitions(-DIOS_SIMULATOR)
		endif()

		add_definitions(-DUSE_ETC_TEXTURE)
	else()
		# MacOS
		set(BUILD_MACOS ON)
		add_definitions(-DMACOS)
	endif()
	
	set(XCODE ON)
	set(USE_OPENMP OFF)
	
	add_definitions(-DUSE_ANGLE_GLES)

	# link_libraries("-lc++")

	if (NOT CMAKE_OSX_ARCHITECTURES)
		# set(CMAKE_OSX_ARCHITECTURES "x86_64")
		set(CMAKE_OSX_ARCHITECTURES "arm64")
	endif()

	# angle api defined
	add_definitions(-DANGLE_USE_UTIL_LOADER)
	add_definitions(-DGL_GLES_PROTOTYPES=0)
	add_definitions(-DEGL_EGL_PROTOTYPES=0)
	add_definitions(-DANGLE_EGL_LIBRARY_NAME=\"libEGL\")
	add_definitions(-DANGLE_GLESV2_LIBRARY_NAME=\"libGLESv2\")
	
	# disable warning
	add_compile_options(-Wno-deprecated-declarations)
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