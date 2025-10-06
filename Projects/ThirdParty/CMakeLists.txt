include_directories(
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/zlib
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/curl/include
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/curl/src
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/freetype2/include
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/libmpg123
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/kdtree
)

if (BUILD_OPENSSL)
include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/boringssl/include)
endif()

#aesGladman
file(GLOB_RECURSE aes_gladman_source 
	./aesGladman/**.cpp 
	./aesGladman/**.hpp 
	./aesGladman/**.c 
	./aesGladman/**.h)
setup_project_group("${aes_gladman_source}" ${CMAKE_CURRENT_SOURCE_DIR})

#bzip2
file(GLOB_RECURSE bzip2_source 
	./bzip2/**.cpp 
	./bzip2/**.hpp 
	./bzip2/**.c 
	./bzip2/**.h)
setup_project_group("${bzip2_source}" ${CMAKE_CURRENT_SOURCE_DIR})

#curl
if (BUILD_CURL)
file(GLOB_RECURSE curl_source 
	./curl/**.cpp
	./curl/**.hpp
	./curl/**.c 
	./curl/**.h)
setup_project_group("${curl_source}" ${CMAKE_CURRENT_SOURCE_DIR})
endif()

#json
file(GLOB_RECURSE json_source 
	./json/**.cpp
	./json/**.hpp
	./json/**.c 
	./json/**.h)
setup_project_group("${json_source}" ${CMAKE_CURRENT_SOURCE_DIR})

#jpeglib
file(GLOB_RECURSE jpeglib_source 
	./jpeglib/**.cpp 
	./jpeglib/**.hpp 
	./jpeglib/**.c 
	./jpeglib/**.h)
setup_project_group("${jpeglib_source}" ${CMAKE_CURRENT_SOURCE_DIR})

#libpng
file(GLOB_RECURSE libpng_source 
	./libpng/**.cpp 
	./libpng/**.hpp 
	./libpng/**.S 
	./libpng/**.c 
	./libpng/**.h)
setup_project_group("${libpng_source}" ${CMAKE_CURRENT_SOURCE_DIR})

#lzma
file(GLOB_RECURSE lzma_source 
	./lzma/**.cpp 
	./lzma/**.hpp 
	./lzma/**.c 
	./lzma/**.h)
setup_project_group("${lzma_source}" ${CMAKE_CURRENT_SOURCE_DIR})

#zlib
file(GLOB_RECURSE zlib_source 
	./zlib/**.cpp 
	./zlib/**.hpp 
	./zlib/**.c 
	./zlib/**.h)
setup_project_group("${zlib_source}" ${CMAKE_CURRENT_SOURCE_DIR})

#freetype2
if (BUILD_FREETYPE)
set(freetype_source
	./freetype2/src/autofit/autofit.c
	./freetype2/src/base/basepic.c
	./freetype2/src/base/ftapi.c
	./freetype2/src/base/ftbase.c
	./freetype2/src/base/ftbbox.c
	./freetype2/src/base/ftbitmap.c
	./freetype2/src/base/ftdbgmem.c
	./freetype2/src/base/ftdebug.c
	./freetype2/src/base/ftglyph.c
	./freetype2/src/base/ftinit.c
	./freetype2/src/base/ftpic.c
	./freetype2/src/base/ftstroke.c
	./freetype2/src/base/ftsynth.c
	./freetype2/src/base/ftsystem.c
	./freetype2/src/cff/cff.c
	./freetype2/src/pshinter/pshinter.c
	./freetype2/src/psaux/psaux.c
	./freetype2/src/psnames/psnames.c
	./freetype2/src/raster/raster.c
	./freetype2/src/sfnt/sfnt.c
	./freetype2/src/smooth/smooth.c
	./freetype2/src/truetype/truetype.c)
add_source_group("${freetype_source}" "source\\freetype2")
endif()

if (BUILD_SKYLICHT_AUDIO)
#libmpg123
file(GLOB_RECURSE libmpg123_source 
	./libmpg123/**.cpp 
	./libmpg123/**.hpp 
	./libmpg123/**.c 
	./libmpg123/**.h
	./libmpg123/**.S)
setup_project_group("${libmpg123_source}" ${CMAKE_CURRENT_SOURCE_DIR})
endif()

#kdtree
file(GLOB_RECURSE kdtree_source 
	./kdtree/**.cpp 
	./kdtree/**.hpp 
	./kdtree/**.c 
	./kdtree/**.h)
setup_project_group("${kdtree_source}" ${CMAKE_CURRENT_SOURCE_DIR})

if (BUILD_SKYLICHT_NETWORK)
file(GLOB_RECURSE easywsclient_source 
	./easywsclient/**.cpp 
	./easywsclient/**.hpp
	./easywsclient/**.h)
setup_project_group("${easywsclient_source}" ${CMAKE_CURRENT_SOURCE_DIR})
endif()

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
	${easywsclient_source}
)

set_target_properties(ThirdParty PROPERTIES VERSION ${SKYLICHT_VERSION})

# OpenSSL
if (BUILD_OPENSSL)
	if (MSVC)
		set(openssl_lib_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/boringssl/lib/Windows/${ARCHITECTURE}")
	elseif (CMAKE_SYSTEM_NAME STREQUAL Android)
		set(openssl_lib_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/boringssl/lib/Android/${ANDROID_ABI}")
	else()
		message(FATAL_ERROR "Please config the SSL library folder.")
	endif()	

	set (SSL_SCOPE PUBLIC)

	if (BUILD_SHARED_LIBS)
	set (SSL_SCOPE PRIVATE)
	endif()	

	if (MSVC)
	target_link_directories(ThirdParty ${SSL_SCOPE} ${openssl_lib_path})
	target_link_libraries(ThirdParty ${SSL_SCOPE} crypto.lib ssl.lib Crypt32.lib)
	else()
	target_link_libraries(ThirdParty ${SSL_SCOPE}
		${openssl_lib_path}/crypto 
		${openssl_lib_path}/ssl)
	endif()
endif()

if (BUILD_SHARED_LIBS)
set_target_properties(ThirdParty PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)

if (BUILD_MACOS)
    # arm link lib C
    add_compile_options(-lc)
endif()
endif()

if (INSTALL_LIBS OR BUILD_SHARED_LIBS)
	if (MSVC OR CYGWIN OR MINGW)
		target_link_libraries(ThirdParty PRIVATE ws2_32)
	endif()
endif()

if (INSTALL_LIBS)
install(TARGETS ThirdParty
			EXPORT ThirdPartyTargets
			RUNTIME DESTINATION ${SKYLICHT_RUNTIME_INSTALL_DIR}
			LIBRARY DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			ARCHIVE DESTINATION ${SKYLICHT_ARCHIVE_INSTALL_DIR})

install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}
	FILES_MATCHING PATTERN "*.h*")

if (BUILD_OPENSSL)
install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/boringssl/include
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/ThirdParty/boringssl
	FILES_MATCHING PATTERN "*.h*")
endif()

install(EXPORT ThirdPartyTargets
	FILE ThirdPartyTargets.cmake
	NAMESPACE Skylicht::
	DESTINATION ${SKYLICHT_TARGET_INSTALL_DIR}
)
endif()
