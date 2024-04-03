INCLUDE_DIRECTORIES( ${BULLET_PHYSICS_SOURCE_DIR}/src  )

ADD_DEFINITIONS(-DB3_USE_CLEW)

SET(Bullet3OpenCL_clew_SRCS
	../clew/clew.c
	BroadphaseCollision/b3GpuGridBroadphase.cpp
	BroadphaseCollision/b3GpuSapBroadphase.cpp
	BroadphaseCollision/b3GpuParallelLinearBvhBroadphase.cpp
	BroadphaseCollision/b3GpuParallelLinearBvh.cpp
	Initialize/b3OpenCLUtils.cpp
	NarrowphaseCollision/b3ContactCache.cpp
	NarrowphaseCollision/b3ConvexHullContact.cpp
	NarrowphaseCollision/b3GjkEpa.cpp
	NarrowphaseCollision/b3OptimizedBvh.cpp
	NarrowphaseCollision/b3QuantizedBvh.cpp
	NarrowphaseCollision/b3StridingMeshInterface.cpp
	NarrowphaseCollision/b3TriangleCallback.cpp
	NarrowphaseCollision/b3TriangleIndexVertexArray.cpp
	NarrowphaseCollision/b3VoronoiSimplexSolver.cpp
	ParallelPrimitives/b3BoundSearchCL.cpp
	ParallelPrimitives/b3FillCL.cpp
	ParallelPrimitives/b3LauncherCL.cpp
	ParallelPrimitives/b3PrefixScanCL.cpp
	ParallelPrimitives/b3PrefixScanFloat4CL.cpp
	ParallelPrimitives/b3RadixSort32CL.cpp
	Raycast/b3GpuRaycast.cpp
	RigidBody/b3GpuGenericConstraint.cpp
	RigidBody/b3GpuJacobiContactSolver.cpp
	RigidBody/b3GpuNarrowPhase.cpp
	RigidBody/b3GpuPgsConstraintSolver.cpp
	RigidBody/b3GpuPgsContactSolver.cpp
	RigidBody/b3GpuRigidBodyPipeline.cpp
	RigidBody/b3Solver.cpp
)


SET(Bullet3OpenCL_clew_HDRS
#	${Root_HDRS}
)


ADD_LIBRARY(Bullet3OpenCL_clew STATIC ${Bullet3OpenCL_clew_SRCS} ${Bullet3OpenCL_clew_HDRS})
SET_TARGET_PROPERTIES(Bullet3OpenCL_clew PROPERTIES VERSION ${BULLET_VERSION})
SET_TARGET_PROPERTIES(Bullet3OpenCL_clew PROPERTIES SOVERSION ${BULLET_VERSION})

TARGET_LINK_LIBRARIES(Bullet3OpenCL_clew LinearMath Bullet3Dynamics ${CMAKE_DL_LIBS})

if (INSTALL_LIBS)
install(TARGETS Bullet3OpenCL_clew
			EXPORT Bullet3OpenCL_clewTargets
			RUNTIME DESTINATION ${LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)

install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DESTINATION ${INCLUDE_INSTALL_DIR}
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT Bullet3OpenCL_clewTargets
	FILE Bullet3OpenCL_clewTargets.cmake
	NAMESPACE Bullet3::
	DESTINATION ${LIBRARY_INSTALL_DIR}/cmake
)

IF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
	SET_TARGET_PROPERTIES(Bullet3OpenCL_clew PROPERTIES FRAMEWORK true)
	SET_TARGET_PROPERTIES(Bullet3OpenCL_clew PROPERTIES PUBLIC_HEADER "${Root_HDRS}")
	# Have to list out sub-directories manually:
	SET_PROPERTY(SOURCE ${BroadphaseCollision_HDRS} PROPERTY MACOSX_PACKAGE_LOCATION Headers/BroadphaseCollision)
ENDIF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
endif()