if (BUILD_EXAMPLES AND BUILD_SKYLICHT_LIGHMAPPER)
	subdirs(Samples/Animations)
	
	if (BUILD_SKYLICHT_AUDIO)
	subdirs(Samples/Audio)
	endif()
	
	subdirs(Samples/BlendShape)
	
	if (BUILD_SKYLICHT_COMPONENTS)
	subdirs(Samples/BoidSystem)
	subdirs(Samples/BoidSystemVAT)
	endif()
	
	if (BUILD_SKYLICHT_COLLISION)
	subdirs(Samples/Collision)
	endif()
	
	subdirs(Samples/DrawPrimitives)

	if (BUILD_SKYLICHT_GRAPH)
	subdirs(Samples/Graph)
	endif()
	
	if (BUILD_SKYLICHT_UI)
	subdirs(Samples/GUI)
	endif()
	
	subdirs(Samples/HelloWorld)
	
	if (BUILD_SKYLICHT_NETWORK)
	subdirs(Samples/HttpRequest)
	endif()
	
	subdirs(Samples/Instancing)
	
	if (BUILD_SKYLICHT_AUDIO)
	subdirs(Samples/LuckyDraw)
	endif()
	
	subdirs(Samples/OcclusionQuery)
	subdirs(Samples/PBR)
	
	if (BUILD_SKYLICHT_PHYSIC)
	subdirs(Samples/CharacterController)
	subdirs(Samples/LoadScene)
	subdirs(Samples/Physics)
	endif()
	
	subdirs(Samples/SkinnedMesh)
	subdirs(Samples/SkinnedMeshInstancing)
	
	if (BUILD_SPINE_RUNTIMES)
	subdirs(Samples/Spine2D)
	endif()
	
	subdirs(Samples/Sponza)
	subdirs(Samples/SpringBone)
	subdirs(Samples/TankScene)
	subdirs(Samples/WalkIK)
	
	if (NOT BUILD_EMSCRIPTEN)
		subdirs(Samples/Lightmap)
		subdirs(Samples/Lightmapping)
		subdirs(Samples/LightmappingDirectional)
		subdirs(Samples/LightmappingVertex)
		subdirs(Samples/LightmapUV)
		
		subdirs(Samples/Materials)
		subdirs(Samples/Noise2D)
		subdirs(Samples/Noise3D)
		
		if (BUILD_SKYLICHT_COMPONENTS)
		subdirs(Samples/Particles)
		subdirs(Samples/ParticlesVortex)
		subdirs(Samples/ParticlesExplosion)
		subdirs(Samples/ParticlesMagicSkill)
		endif()
		
		subdirs(Samples/Shader)
		
		if (BUILD_SKYLICHT_NETWORK)
		subdirs(Samples/SocketIO)
		endif()
	endif()	
endif()