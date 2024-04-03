
INCLUDE_DIRECTORIES(
	${BULLET_PHYSICS_SOURCE_DIR}/src
)

SET(Bullet3Collision_SRCS
	BroadPhaseCollision/b3DynamicBvh.cpp
	BroadPhaseCollision/b3DynamicBvhBroadphase.cpp
	BroadPhaseCollision/b3OverlappingPairCache.cpp
	NarrowPhaseCollision/b3ConvexUtility.cpp
	NarrowPhaseCollision/b3CpuNarrowPhase.cpp
)

SET(Bullet3CollisionBroadPhase_HDRS
	BroadPhaseCollision/b3BroadphaseCallback.h
	BroadPhaseCollision/b3DynamicBvh.h
	BroadPhaseCollision/b3DynamicBvhBroadphase.h
	BroadPhaseCollision/b3OverlappingPair.h
	BroadPhaseCollision/b3OverlappingPairCache.h
)
SET(Bullet3CollisionBroadPhaseShared_HDRS
	BroadPhaseCollision/shared/b3Aabb.h
)

SET(Bullet3CollisionNarrowPhase_HDRS
	NarrowPhaseCollision/b3Config.h
	NarrowPhaseCollision/b3Contact4.h
	NarrowPhaseCollision/b3ConvexUtility.h
	NarrowPhaseCollision/b3CpuNarrowPhase.h
	NarrowPhaseCollision/b3RaycastInfo.h
	NarrowPhaseCollision/b3RigidBodyCL.h
)
SET(Bullet3CollisionNarrowPhaseShared_HDRS

	NarrowPhaseCollision/shared/b3BvhSubtreeInfoData.h
	NarrowPhaseCollision/shared/b3BvhTraversal.h
	NarrowPhaseCollision/shared/b3ClipFaces.h
	NarrowPhaseCollision/shared/b3Collidable.h
	NarrowPhaseCollision/shared/b3Contact4Data.h
	NarrowPhaseCollision/shared/b3ContactConvexConvexSAT.h
	NarrowPhaseCollision/shared/b3ContactSphereSphere.h
	NarrowPhaseCollision/shared/b3ConvexPolyhedronData.h
	NarrowPhaseCollision/shared/b3FindConcaveSatAxis.h
	NarrowPhaseCollision/shared/b3FindSeparatingAxis.h
	NarrowPhaseCollision/shared/b3MprPenetration.h
	NarrowPhaseCollision/shared/b3NewContactReduction.h
	NarrowPhaseCollision/shared/b3QuantizedBvhNodeData.h
	NarrowPhaseCollision/shared/b3ReduceContacts.h
	NarrowPhaseCollision/shared/b3RigidBodyData.h
	NarrowPhaseCollision/shared/b3UpdateAabbs.h
)

SET(Bullet3Collision_HDRS
	${Bullet3CollisionBroadPhase_HDRS}
	${Bullet3CollisionBroadPhaseShared_HDRS}
	${Bullet3CollisionNarrowPhaseShared_HDRS}
	${Bullet3CollisionNarrowPhase_HDRS}
)

ADD_LIBRARY(Bullet3Collision STATIC ${Bullet3Collision_SRCS} ${Bullet3Collision_HDRS})

target_link_libraries(Bullet3Collision Bullet3Geometry)

SET_TARGET_PROPERTIES(Bullet3Collision PROPERTIES VERSION ${BULLET_VERSION})
SET_TARGET_PROPERTIES(Bullet3Collision PROPERTIES SOVERSION ${BULLET_VERSION})

if (INSTALL_LIBS)
install(TARGETS Bullet3Collision
			EXPORT Bullet3CollisionTargets
			RUNTIME DESTINATION ${LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)

install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DESTINATION ${INCLUDE_INSTALL_DIR}
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT Bullet3CollisionTargets
	FILE Bullet3CollisionTargets.cmake
	NAMESPACE Bullet3::
	DESTINATION ${LIBRARY_INSTALL_DIR}/cmake
)

IF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
	SET_TARGET_PROPERTIES(Bullet3Collision PROPERTIES FRAMEWORK true)
	SET_TARGET_PROPERTIES(Bullet3Collision PROPERTIES PUBLIC_HEADER "${Bullet3Collision_HDRS}")
ENDIF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
endif()