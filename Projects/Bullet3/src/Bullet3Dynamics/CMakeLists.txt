
INCLUDE_DIRECTORIES(
	${BULLET_PHYSICS_SOURCE_DIR}/src
)

SET(Bullet3Dynamics_SRCS
	b3CpuRigidBodyPipeline.cpp
	ConstraintSolver/b3FixedConstraint.cpp
	ConstraintSolver/b3Generic6DofConstraint.cpp
	ConstraintSolver/b3PgsJacobiSolver.cpp
	ConstraintSolver/b3Point2PointConstraint.cpp
	ConstraintSolver/b3TypedConstraint.cpp
)

SET(Bullet3Dynamics_HDRS
	  b3CpuRigidBodyPipeline.h
	ConstraintSolver/b3ContactSolverInfo.h
	ConstraintSolver/b3FixedConstraint.h
	ConstraintSolver/b3Generic6DofConstraint.h
	ConstraintSolver/b3JacobianEntry.h
	ConstraintSolver/b3PgsJacobiSolver.h
	ConstraintSolver/b3Point2PointConstraint.h
	ConstraintSolver/b3SolverBody.h
	ConstraintSolver/b3SolverConstraint.h
	ConstraintSolver/b3TypedConstraint.h
	shared/b3ContactConstraint4.h
	shared/b3ConvertConstraint4.h
	shared/b3Inertia.h
	shared/b3IntegrateTransforms.h
)

ADD_LIBRARY(Bullet3Dynamics STATIC ${Bullet3Dynamics_SRCS} ${Bullet3Dynamics_HDRS})

target_link_libraries(Bullet3Dynamics Bullet3Collision)

SET_TARGET_PROPERTIES(Bullet3Dynamics PROPERTIES VERSION ${BULLET_VERSION})
SET_TARGET_PROPERTIES(Bullet3Dynamics PROPERTIES SOVERSION ${BULLET_VERSION})

if (INSTALL_LIBS)
install(TARGETS Bullet3Dynamics
			EXPORT Bullet3DynamicsTargets
			RUNTIME DESTINATION ${LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)

install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DESTINATION ${INCLUDE_INSTALL_DIR}
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT Bullet3DynamicsTargets
	FILE Bullet3DynamicsTargets.cmake
	NAMESPACE Bullet3::
	DESTINATION ${LIBRARY_INSTALL_DIR}/cmake
)

IF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
	SET_TARGET_PROPERTIES(Bullet3Dynamics PROPERTIES FRAMEWORK true)
	SET_TARGET_PROPERTIES(Bullet3Dynamics PROPERTIES PUBLIC_HEADER "${Bullet3Dynamics_HDRS}")
ENDIF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
endif()