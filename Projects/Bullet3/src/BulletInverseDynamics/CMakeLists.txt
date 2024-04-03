INCLUDE_DIRECTORIES( ${BULLET_PHYSICS_SOURCE_DIR}/src  )

SET(BulletInverseDynamics_SRCS
	IDMath.cpp
	MultiBodyTree.cpp
	details/MultiBodyTreeInitCache.cpp
	details/MultiBodyTreeImpl.cpp
)

SET(BulletInverseDynamicsRoot_HDRS
	IDConfig.hpp
	IDConfigEigen.hpp
	IDMath.hpp
	IDConfigBuiltin.hpp
	IDErrorMessages.hpp
	MultiBodyTree.hpp
)
SET(BulletInverseDynamicsDetails_HDRS
	details/IDEigenInterface.hpp
	details/IDMatVec.hpp
	details/IDLinearMathInterface.hpp
	details/MultiBodyTreeImpl.hpp
	details/MultiBodyTreeInitCache.hpp
)

SET(BulletInverseDynamics_HDRS
	${BulletInverseDynamicsRoot_HDRS}
	${BulletInverseDynamicsDetails_HDRS}
)


ADD_LIBRARY(BulletInverseDynamics STATIC ${BulletInverseDynamics_SRCS} ${BulletInverseDynamics_HDRS})
SET_TARGET_PROPERTIES(BulletInverseDynamics PROPERTIES VERSION ${BULLET_VERSION})
SET_TARGET_PROPERTIES(BulletInverseDynamics PROPERTIES SOVERSION ${BULLET_VERSION})

TARGET_LINK_LIBRARIES(BulletInverseDynamics Bullet3Common LinearMath)

if (INSTALL_LIBS)
install(TARGETS BulletInverseDynamics
			EXPORT BulletInverseDynamicsTargets
			RUNTIME DESTINATION ${LIBRARY_INSTALL_DIR}
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)

install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DESTINATION ${INCLUDE_INSTALL_DIR}
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT BulletInverseDynamicsTargets
	FILE BulletInverseDynamicsTargets.cmake
	NAMESPACE Bullet3::
	DESTINATION ${LIBRARY_INSTALL_DIR}/cmake
)

IF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
	SET_TARGET_PROPERTIES(BulletInverseDynamics PROPERTIES FRAMEWORK true)
	SET_TARGET_PROPERTIES(BulletInverseDynamics PROPERTIES PUBLIC_HEADER "${BulletInverseDynamicsRoot_HDRS}")
	# Have to list out sub-directories manually:
	SET_PROPERTY(SOURCE ${BulletInverseDynamicsDetails_HDRS} PROPERTY MACOSX_PACKAGE_LOCATION Headers/details)
ENDIF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
endif()