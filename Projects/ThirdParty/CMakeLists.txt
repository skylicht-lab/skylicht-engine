include_directories(	
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/zlib
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/curl/include
)

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
file(GLOB_RECURSE curl_source 
	./source/curl/**.cpp 
	./source/curl/**.hpp 
	./source/curl/**.c 
	./source/curl/**.h)
setup_project_group("${curl_source}" ${CMAKE_CURRENT_SOURCE_DIR})

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

#json
file(GLOB_RECURSE json_source 
	./source/json/**.cpp 
	./source/json/**.hpp 
	./source/json/**.c 
	./source/json/**.h)
setup_project_group("${json_source}" ${CMAKE_CURRENT_SOURCE_DIR})

#lzma
file(GLOB_RECURSE lzma_source 
	./source/lzma/**.cpp 
	./source/lzma/**.hpp 
	./source/lzma/**.c 
	./source/lzma/**.h)
setup_project_group("${lzma_source}" ${CMAKE_CURRENT_SOURCE_DIR})

#mpc
file(GLOB_RECURSE mpc_source 
	./source/mpc/**.cpp 
	./source/mpc/**.hpp 
	./source/mpc/**.c 
	./source/mpc/**.h)
setup_project_group("${mpc_source}" ${CMAKE_CURRENT_SOURCE_DIR})

#zlib
file(GLOB_RECURSE zlib_source 
	./source/zlib/**.cpp 
	./source/zlib/**.hpp 
	./source/zlib/**.c 
	./source/zlib/**.h)
setup_project_group("${zlib_source}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(ThirdParty STATIC
	${aes_gladman_source} 
	${bzip2_source}
	${curl_source}
	${jpeglib_source}
	${json_source}
	${libpng_source}
	${lzma_source}
	${mpc_source}
	${zlib_source}
)

set_target_properties(ThirdParty PROPERTIES VERSION ${SKYLICHT_VERSION})