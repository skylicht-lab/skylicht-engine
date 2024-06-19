if (BUILD_EXAMPLES AND BUILD_SKYLICHT_LIGHMAPPER)
	if (BUILD_SKYLICHT_AUDIO)
	subdirs(Samples/Audio)
	endif()
	
	subdirs(Samples/Animations)
	subdirs(Samples/BlendShape)
	
	if (BUILD_SKYLICHT_COMPONENTS)
	subdirs(Samples/BoidSystem)
	subdirs(Samples/BoidSystemVAT)
	endif()
	
	subdirs(Samples/HelloWorld)
	
	if (BUILD_SKYLICHT_COLLISION)
	subdirs(Samples/Collision)
	endif()
	
	subdirs(Samples/DrawPrimitives)
	subdirs(Samples/Instancing)
	
	if (BUILD_SKYLICHT_AUDIO)
	subdirs(Samples/LuckyDraw)
	endif()
	
	subdirs(Samples/OcclusionQuery)
	subdirs(Samples/PBR)
	
	if (BUILD_SKYLICHT_PHYSIC)
	subdirs(Samples/Physics)
	endif()
	
	subdirs(Samples/Sponza)
	subdirs(Samples/SkinnedMesh)
	subdirs(Samples/SkinnedMeshInstancing)
	subdirs(Samples/TankScene)
	subdirs(Samples/WalkIK)
	
	if (NOT BUILD_EMSCRIPTEN)
		subdirs(Samples/LightmapUV)
		subdirs(Samples/Lightmapping)
		subdirs(Samples/LightmappingVertex)
		subdirs(Samples/LightmappingDirectional)
		subdirs(Samples/Lightmap)
		subdirs(Samples/Materials)
		subdirs(Samples/Noise2D)
		subdirs(Samples/Noise3D)
		
		if (BUILD_SKYLICHT_COMPONENTS)
		subdirs(Samples/Particles)
		subdirs(Samples/ParticlesVortex)
		subdirs(Samples/ParticlesExplosion)
		subdirs(Samples/ParticlesMagicSkill)
		endif()
		
		if (BUILD_SKYLICHT_NETWORK)
		subdirs(Samples/SocketIO)
		endif()
		
		subdirs(Samples/Shader)
	endif()	
endif()