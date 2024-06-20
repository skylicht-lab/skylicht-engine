if (CMAKE_SYSTEM_NAME STREQUAL WindowsStore)
	set(BUILD_ENGINE_TOOL OFF)
	set(BUILD_WINDOWS_STORE ON)
	add_definitions(-DWINDOWS_STORE)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
	set(BUILD_ENGINE_TOOL OFF)
	set(BUILD_EMSCRIPTEN ON)
	set(USE_OPENMP OFF)
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
	set(CMAKE_SKIP_RPATH TRUE)
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