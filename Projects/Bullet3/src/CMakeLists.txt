
IF(BUILD_BULLET3)
	SUBDIRS(  Bullet3OpenCL Bullet3Serialize/Bullet2FileLoader Bullet3Dynamics Bullet3Collision Bullet3Geometry )
ENDIF(BUILD_BULLET3)


SUBDIRS(   BulletInverseDynamics BulletSoftBody BulletCollision BulletDynamics LinearMath Bullet3Common)


IF(INSTALL_LIBS)
	INSTALL(FILES btBulletCollisionCommon.h btBulletDynamicsCommon.h  DESTINATION ${INCLUDE_INSTALL_DIR})
ENDIF(INSTALL_LIBS)
