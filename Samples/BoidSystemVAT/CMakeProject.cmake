include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Samples/SampleFW
	${SKYLICHT_ENGINE_SOURCE_DIR}/Samples/BoidSystem/Source/Boids
)

file(GLOB_RECURSE additional_source
	../SampleFW/**.cpp
	../SampleFW/**.h
	../BoidSystem/Source/Boids/**.cpp
	../BoidSystem/Source/Boids/**.h)
	
list (APPEND application_source ${additional_source})