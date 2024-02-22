include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Physics/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Irrlicht/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/ThirdParty/source/curl/include
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/System/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Audio/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Skylicht/Engine/Source
	${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Bullet3/src
)

if (BUILD_BULLET_PHYSIC_LIB)
include_directories(${SKYLICHT_ENGINE_SOURCE_DIR}/Projects/Bullet3/src)
endif()

file(GLOB_RECURSE skylicht_physics 
	./Source/*.cpp 
	./Source/*.hpp 
	./Source/*.c 
	./Source/*.h)
setup_project_group("${skylicht_physics}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Physics STATIC
	${skylicht_physics}
)

target_precompiled_header(Physics ./Source/pch.cpp ${skylicht_physics})

set_target_properties(Physics PROPERTIES VERSION ${SKYLICHT_VERSION})

target_link_libraries(Physics Engine System)

if (BUILD_BULLET_PHYSIC_LIB)
target_link_libraries(Physics LinearMath Bullet3Common BulletCollision BulletDynamics BulletInverseDynamics BulletSoftBody)

IF(BUILD_BULLET3)
target_link_libraries(Physics Bullet3OpenCL Bullet2FileLoader Bullet3Collision Bullet3Dynamics Bullet3Geometry)
ENDIF(BUILD_BULLET3)

endif()