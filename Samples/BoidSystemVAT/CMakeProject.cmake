include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Samples/SampleFW
)

file(GLOB_RECURSE additional_source
	../SampleFW/**.cpp
	../SampleFW/**.h)
	
list (APPEND application_source ${additional_source})