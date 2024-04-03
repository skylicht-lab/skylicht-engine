
INCLUDE_DIRECTORIES(
${BULLET_PHYSICS_SOURCE_DIR}/src

)

#SUBDIRS( Solvers )

SET(BulletSoftBody_SRCS
	btSoftBody.cpp
	btSoftBodyConcaveCollisionAlgorithm.cpp
	btSoftBodyHelpers.cpp
	btSoftBodyRigidBodyCollisionConfiguration.cpp
	btSoftRigidCollisionAlgorithm.cpp
	btSoftRigidDynamicsWorld.cpp
	btSoftMultiBodyDynamicsWorld.cpp
	btSoftSoftCollisionAlgorithm.cpp
	btDefaultSoftBodySolver.cpp

	btDeformableBackwardEulerObjective.cpp
	btDeformableBodySolver.cpp
	btDeformableMultiBodyConstraintSolver.cpp
	btDeformableContactProjection.cpp
	btDeformableMultiBodyDynamicsWorld.cpp
	btDeformableContactConstraint.cpp
	poly34.cpp

	BulletReducedDeformableBody/btReducedDeformableBody.cpp
	BulletReducedDeformableBody/btReducedDeformableBodyHelpers.cpp
	BulletReducedDeformableBody/btReducedDeformableBodySolver.cpp
	BulletReducedDeformableBody/btReducedDeformableContactConstraint.cpp
)


SET(BulletSoftBody_HDRS
	btSoftBody.h
	btSoftBodyData.h
	btSoftBodyConcaveCollisionAlgorithm.h
	btSoftBodyHelpers.h
	btSoftBodyRigidBodyCollisionConfiguration.h
	btSoftRigidCollisionAlgorithm.h
	btSoftRigidDynamicsWorld.h
	btSoftMultiBodyDynamicsWorld.h
	btSoftSoftCollisionAlgorithm.h
	btSparseSDF.h

	btSoftBodySolvers.h
	btDefaultSoftBodySolver.h
	
	btCGProjection.h
	btConjugateGradient.h
	btConjugateResidual.h
	btDeformableGravityForce.h
	btDeformableMassSpringForce.h
	btDeformableCorotatedForce.h
	btDeformableNeoHookeanForce.h
	btDeformableLinearElasticityForce.h
	btDeformableLagrangianForce.h
	btPreconditioner.h

	btDeformableBackwardEulerObjective.h
	btDeformableBodySolver.h
	btDeformableMultiBodyConstraintSolver.h
	btDeformableContactProjection.h
	btDeformableMultiBodyDynamicsWorld.h
	btDeformableContactConstraint.h
	btKrylovSolver.h
	poly34.h

	btSoftBodySolverVertexBuffer.h

	BulletReducedDeformableBody/btReducedDeformableBody.h
	BulletReducedDeformableBody/btReducedDeformableBodyHelpers.h
	BulletReducedDeformableBody/btReducedDeformableBodySolver.h
	BulletReducedDeformableBody/btReducedDeformableContactConstraint.h
)



ADD_LIBRARY(BulletSoftBody STATIC ${BulletSoftBody_SRCS} ${BulletSoftBody_HDRS})
SET_TARGET_PROPERTIES(BulletSoftBody PROPERTIES VERSION ${BULLET_VERSION})
SET_TARGET_PROPERTIES(BulletSoftBody PROPERTIES SOVERSION ${BULLET_VERSION})

TARGET_LINK_LIBRARIES(BulletSoftBody BulletDynamics)

if (INSTALL_LIBS)
install(TARGETS BulletSoftBody
			EXPORT BulletSoftBodyTargets
			RUNTIME DESTINATION ${LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)

install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DESTINATION ${INCLUDE_INSTALL_DIR}
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT BulletSoftBodyTargets
	FILE BulletSoftBodyTargets.cmake
	NAMESPACE Bullet3::
	DESTINATION ${LIBRARY_INSTALL_DIR}/cmake
)

IF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
	SET_TARGET_PROPERTIES(BulletSoftBody PROPERTIES FRAMEWORK true)
	SET_TARGET_PROPERTIES(BulletSoftBody PROPERTIES PUBLIC_HEADER "${BulletSoftBody_HDRS}")
ENDIF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
endif()