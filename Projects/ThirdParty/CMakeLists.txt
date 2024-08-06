include_directories(
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/source
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/source/zlib
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/source/curl/include
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/source/curl/src
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/source/freetype2/include
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/source/libmpg123
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/source/kdtree
)

if (BUILD_OPENSSL)
include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/boringssl/include)
endif()

#aesGladman
file(GLOB_RECURSE aes_gladman_source 
	./source/aesGladman/**.cpp 
	./source/aesGladman/**.hpp 
	./source/aesGladman/**.c 
	./source/aesGladman/**.h)
setup_project_group("${aes_gladman_source}" ${CMAKE_CURRENT_SOURCE_DIR})

#bzip2
file(GLOB_RECURSE bzip2_source 
	./source/bzip2/**.cpp 
	./source/bzip2/**.hpp 
	./source/bzip2/**.c 
	./source/bzip2/**.h)
setup_project_group("${bzip2_source}" ${CMAKE_CURRENT_SOURCE_DIR})

#curl
if (BUILD_CURL)
file(GLOB_RECURSE curl_source 
	./source/curl/**.cpp 
	./source/curl/**.hpp 
	./source/curl/**.c 
	./source/curl/**.h)
setup_project_group("${curl_source}" ${CMAKE_CURRENT_SOURCE_DIR})

#json
file(GLOB_RECURSE json_source 
	./source/json/**.cpp 
	./source/json/**.hpp 
	./source/json/**.c 
	./source/json/**.h)
setup_project_group("${json_source}" ${CMAKE_CURRENT_SOURCE_DIR})
endif()

#jpeglib
file(GLOB_RECURSE jpeglib_source 
	./source/jpeglib/**.cpp 
	./source/jpeglib/**.hpp 
	./source/jpeglib/**.c 
	./source/jpeglib/**.h)
setup_project_group("${jpeglib_source}" ${CMAKE_CURRENT_SOURCE_DIR})

#libpng
file(GLOB_RECURSE libpng_source 
	./source/libpng/**.cpp 
	./source/libpng/**.hpp 
	./source/libpng/**.c 
	./source/libpng/**.h)
setup_project_group("${libpng_source}" ${CMAKE_CURRENT_SOURCE_DIR})

#lzma
file(GLOB_RECURSE lzma_source 
	./source/lzma/**.cpp 
	./source/lzma/**.hpp 
	./source/lzma/**.c 
	./source/lzma/**.h)
setup_project_group("${lzma_source}" ${CMAKE_CURRENT_SOURCE_DIR})

#zlib
file(GLOB_RECURSE zlib_source 
	./source/zlib/**.cpp 
	./source/zlib/**.hpp 
	./source/zlib/**.c 
	./source/zlib/**.h)
setup_project_group("${zlib_source}" ${CMAKE_CURRENT_SOURCE_DIR})

#freetype2
if (BUILD_FREETYPE)
set(freetype_source
	./source/freetype2/src/autofit/autofit.c
	./source/freetype2/src/base/basepic.c
	./source/freetype2/src/base/ftapi.c
	./source/freetype2/src/base/ftbase.c
	./source/freetype2/src/base/ftbbox.c
	./source/freetype2/src/base/ftbitmap.c
	./source/freetype2/src/base/ftdbgmem.c
	./source/freetype2/src/base/ftdebug.c
	./source/freetype2/src/base/ftglyph.c
	./source/freetype2/src/base/ftinit.c
	./source/freetype2/src/base/ftpic.c
	./source/freetype2/src/base/ftstroke.c
	./source/freetype2/src/base/ftsynth.c
	./source/freetype2/src/base/ftsystem.c
	./source/freetype2/src/cff/cff.c
	./source/freetype2/src/pshinter/pshinter.c
	./source/freetype2/src/psaux/psaux.c
	./source/freetype2/src/psnames/psnames.c
	./source/freetype2/src/raster/raster.c
	./source/freetype2/src/sfnt/sfnt.c
	./source/freetype2/src/smooth/smooth.c
	./source/freetype2/src/truetype/truetype.c)
add_source_group("${freetype_source}" "source\\freetype2")
endif()

if (BUILD_SKYLICHT_AUDIO)
#libmpg123
file(GLOB_RECURSE libmpg123_source 
	./source/libmpg123/**.cpp 
	./source/libmpg123/**.hpp 
	./source/libmpg123/**.c 
	./source/libmpg123/**.h
	./source/libmpg123/**.S)
setup_project_group("${libmpg123_source}" ${CMAKE_CURRENT_SOURCE_DIR})
endif()

#kdtree
file(GLOB_RECURSE kdtree_source 
	./source/kdtree/**.cpp 
	./source/kdtree/**.hpp 
	./source/kdtree/**.c 
	./source/kdtree/**.h)
setup_project_group("${kdtree_source}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(ThirdParty ${ENGINE_SHARED_OR_STATIC_LIB}
	${aes_gladman_source} 
	${bzip2_source}
	${curl_source}
	${jpeglib_source}
	${json_source}
	${libpng_source}
	${lzma_source}
	${mpc_source}
	${zlib_source}
	${freetype_source}
	${libmpg123_source}
	${kdtree_source}
)

set_target_properties(ThirdParty PROPERTIES VERSION ${SKYLICHT_VERSION})

if (BUILD_SHARED_LIBS)
set_target_properties(ThirdParty PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)

if (BUILD_MACOS)
    # arm link lib C
    add_compile_options(-lc)
endif()
endif()

if (MSVC OR CYGWIN OR MINGW)
	target_link_libraries(ThirdParty PRIVATE ws2_32)
endif()

if (BUILD_OPENSSL AND MSVC)
	target_link_libraries(ThirdParty PRIVATE crypto.lib decrepit.lib ssl.lib Crypt32.lib)
	if (CMAKE_CL_64)
		set(openssl_lib_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/boringssl/x64")
		set(openssl_dll_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/boringssl/x64/*.dll")
	else()
		set(openssl_lib_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/boringssl/x86")
		set(openssl_dll_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/boringssl/x86/*.dll")
	endif()
	target_link_directories(ThirdParty PRIVATE ${openssl_lib_path})
endif()

if (INSTALL_LIBS)
install(TARGETS ThirdParty
			EXPORT ThirdPartyTargets
			RUNTIME DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)

install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/source
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/ThirdParty
	FILES_MATCHING PATTERN "*.h*")

if (BUILD_OPENSSL)
install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/boringssl/include
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/ThirdParty/source/boringssl
	FILES_MATCHING PATTERN "*.h*")
endif()

install(EXPORT ThirdPartyTargets
	FILE ThirdPartyTargets.cmake
	NAMESPACE Skylicht::
	DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}/cmake
)
endif()
