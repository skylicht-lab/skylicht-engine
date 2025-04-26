/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CShaderManager.h"
#include "CShader.h"
#include "Utils/CPath.h"

#include "Instancing/CStandardSGInstancing.h"
#include "Instancing/CTBNSGInstancing.h"
#include "Instancing/CStandardColorInstancing.h"

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CShaderManager);

	CShaderManager::CShaderManager() :
		m_currentMeshBuffer(NULL),
		m_currentMatRendering(NULL),
		BoneMatrix(NULL),
		BoneCount(0),
		LightmapIndex(0)
	{
	}

	CShaderManager::~CShaderManager()
	{
		releaseAll();
	}

	void CShaderManager::releaseAll()
	{
		for (u32 i = 0, n = (u32)m_listShader.size(); i < n; i++)
			m_listShader[i]->drop();

		m_listShader.clear();
	}

	std::string CShaderManager::getShaderFileName(const char* fileName)
	{
		std::string ret = fileName;

		IVideoDriver* driver = getVideoDriver();
		if (driver->getDriverType() == video::EDT_DIRECT3D11)
			ret += std::string(".hlsl");
		else
			ret += std::string(".glsl");

		return ret;
	}

	void CShaderManager::initGUIShader()
	{
		loadShader("BuiltIn/Shader/Basic/VertexColor.xml");
		loadShader("BuiltIn/Shader/Basic/VertexColorAlpha.xml");
		loadShader("BuiltIn/Shader/Basic/VertexColorAdditive.xml");

		loadShader("BuiltIn/Shader/Basic/TextureColor.xml");
		loadShader("BuiltIn/Shader/Basic/TextureColorAlpha.xml");
		loadShader("BuiltIn/Shader/Basic/TextureColorAlphaBGR.xml");
		loadShader("BuiltIn/Shader/Basic/TextureColorAlphaBW.xml");
	}

	void CShaderManager::initBasicShader()
	{
		loadShader("BuiltIn/Shader/Basic/VertexColor.xml");
		loadShader("BuiltIn/Shader/Basic/VertexColorAlpha.xml");
		loadShader("BuiltIn/Shader/Basic/VertexColorAdditive.xml");

		loadShader("BuiltIn/Shader/Basic/TextureColor.xml");
		loadShader("BuiltIn/Shader/Basic/TextureColorAlpha.xml");
		loadShader("BuiltIn/Shader/Basic/TextureColorAlphaBGR.xml");
		loadShader("BuiltIn/Shader/Basic/TextureColorAlphaBW.xml");

		loadShader("BuiltIn/Shader/Basic/TextureColorAdditive.xml");
		loadShader("BuiltIn/Shader/Basic/TextureColor2LayerAdditive.xml");
		loadShader("BuiltIn/Shader/Basic/TextureColorMultiply.xml");
		loadShader("BuiltIn/Shader/Basic/TextureColorScreen.xml");

		loadShader("BuiltIn/Shader/Basic/AlphaTest.xml");
		loadShader("BuiltIn/Shader/Basic/AlphaBlend.xml");

		loadShader("BuiltIn/Shader/Basic/Skin.xml");
		loadShader("BuiltIn/Shader/Basic/SkinVertexColor.xml");

		loadShader("BuiltIn/Shader/ShadowDepthWrite/ShadowDepthWrite.xml");
		loadShader("BuiltIn/Shader/ShadowDepthWrite/ShadowDepthWriteSkinMesh.xml");
		loadShader("BuiltIn/Shader/ShadowDepthWrite/ShadowLightDistanceWrite.xml");
		loadShader("BuiltIn/Shader/ShadowDepthWrite/ShadowLightDistanceWriteSkinMesh.xml");

		loadShader("BuiltIn/Shader/ShadowDepthWrite/SDWStandardSGInstancing.xml");
		loadShader("BuiltIn/Shader/ShadowDepthWrite/SDWTangentSGInstancing.xml");
		loadShader("BuiltIn/Shader/ShadowDepthWrite/SDWSkinInstancing.xml");

		loadShader("BuiltIn/Shader/ShadowDepthWrite/SDWDistanceStandardSGInstancing.xml");
		loadShader("BuiltIn/Shader/ShadowDepthWrite/SDWDistanceTangentSGInstancing.xml");

		loadShader("BuiltIn/Shader/Basic/TextureSRGB.xml");
		loadShader("BuiltIn/Shader/Basic/TextureLinearRGB.xml");
		loadShader("BuiltIn/Shader/Basic/Luminance.xml");

		loadShader("BuiltIn/Shader/Lightmap/Lightmap.xml");
		loadShader("BuiltIn/Shader/Lightmap/LightmapUV.xml");
		loadShader("BuiltIn/Shader/Lightmap/LightmapDirection.xml");
		loadShader("BuiltIn/Shader/Lightmap/IndirectTest.xml");
		loadShader("BuiltIn/Shader/Lightmap/LightmapVertex.xml");
		loadShader("BuiltIn/Shader/Lightmap/LightmapSH.xml");
		loadShader("BuiltIn/Shader/Lightmap/LightmapColor.xml");

		loadShader("BuiltIn/Shader/PostProcessing/AdaptLuminance.xml");
		loadShader("BuiltIn/Shader/PostProcessing/PostEffect.xml");
		loadShader("BuiltIn/Shader/PostProcessing/PostEffectManualExposure.xml");

		loadShader("BuiltIn/Shader/Particle/ParticleAdditive.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleTransparent.xml");

		loadShader("BuiltIn/Shader/Particle/ParticleBillboardAdditive.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleBillboardAdditiveAlpha.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleBillboardTransparent.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleBillboardTransparentAlpha.xml");

		loadShader("BuiltIn/Shader/Particle/ParticleVelocityAdditive.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleVelocityAdditiveAlpha.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleVelocityTransparent.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleVelocityTransparentAlpha.xml");

		loadShader("BuiltIn/Shader/Particle/ParticleOrientationAdditive.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleOrientationAdditiveAlpha.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleOrientationTransparent.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleOrientationTransparentAlpha.xml");

		loadShader("BuiltIn/Shader/Particle/ParticleTrailTurbulenceAdditive.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleTrailTurbulenceAdditiveAlpha.xml");

		loadShader("BuiltIn/Shader/Particle/ParticleMesh.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleMeshColor.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleMeshAddtive.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleMeshTransparent.xml");

		loadShader("BuiltIn/Shader/SkySun/SkySun.xml");
	}

	void CShaderManager::initSGDeferredShader()
	{
		loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/ColorInstancing.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/MetallicRoughnessInstancing.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/SpecularGlossinessInstancing.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/DiffuseNormalInstancing.xml");

		loadShader("BuiltIn/Shader/Lightmap/LMStandardSGInstancing.xml");
		loadShader("BuiltIn/Shader/Lightmap/LMTBNSGInstancing.xml");

		loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Color.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/DiffuseNormal.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Specular.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Diffuse.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/SpecularGlossiness.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/SpecularGlossinessMask.xml");

		loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/MetallicRoughness.xml");

		loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/MetersGrid.xml");

		loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGLightmap.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGDirectionalLight.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGDirectionalLightSSR.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGDirectionalLightBake.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGPointLight.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGPointLightShadow.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGSpotLight.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Forward/SH.xml");
	}

	void CShaderManager::initSGForwarderShader()
	{
		loadShader("BuiltIn/Shader/SpecularGlossiness/Forward/ReflectionProbe.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Forward/SG.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Forward/SGColor.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Forward/SGDiffuse.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Forward/SGNoNormalMap.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Forward/SGSkin.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Forward/SGSkinAlpha.xml");
	}

	void CShaderManager::initMobileSGShader()
	{
		loadShader("BuiltIn/Shader/Mobile/MobileSG.xml");
		loadShader("BuiltIn/Shader/Mobile/MobileSGColor.xml");
		loadShader("BuiltIn/Shader/Mobile/MobileSGDiffuse.xml");
		loadShader("BuiltIn/Shader/Mobile/MobileSGNoNormalMap.xml");
		loadShader("BuiltIn/Shader/Mobile/MobileSGNoNormalMapAO.xml");

		loadShader("BuiltIn/Shader/Mobile/MobileSGShadow.xml");
		loadShader("BuiltIn/Shader/Mobile/MobileSGDiffuseShadow.xml");

		loadShader("BuiltIn/Shader/Mobile/MobileSGSkin.xml");
		loadShader("BuiltIn/Shader/Mobile/MobileSGSkinAlpha.xml");
	}

	void CShaderManager::initPBRForwarderShader()
	{
		loadShader("BuiltIn/Shader/PBR/Forward/PBR.xml");
		loadShader("BuiltIn/Shader/PBR/Forward/PBRNoEmissive.xml");
		loadShader("BuiltIn/Shader/PBR/Forward/PBRNoNormalMap.xml");
		loadShader("BuiltIn/Shader/PBR/Forward/PBRNoNormalMapNoEmissve.xml");
		loadShader("BuiltIn/Shader/PBR/Forward/PBRNoTexture.xml");

		loadShader("BuiltIn/Shader/PBR/Forward/PBRSkin.xml");
		loadShader("BuiltIn/Shader/PBR/Forward/PBRSkinNoEmissive.xml");
		loadShader("BuiltIn/Shader/PBR/Forward/PBRSkinNoNormal.xml");
		loadShader("BuiltIn/Shader/PBR/Forward/PBRSkinNoNormalNoEmissive.xml");
	}

	void CShaderManager::initSkylichtEngineShader()
	{
		initSGDeferredShader();
		initSGForwarderShader();
	}


	void CShaderManager::initShader()
	{
		initBasicShader();
		initSkylichtEngineShader();
	}

	CShader* CShaderManager::loadShader(const char* shaderConfig)
	{
		char log[512];

		io::IXMLReader* xmlReader = getIrrlichtDevice()->getFileSystem()->createXMLReader(shaderConfig);
		if (xmlReader == NULL)
		{
			sprintf(log, "Load shader: %s - File not found", shaderConfig);
			os::Printer::log(log);
			return NULL;
		}

		sprintf(log, "Load shader: %s", shaderConfig);
		os::Printer::log(log);

		std::string shaderFolder = CPath::getFolderPath(std::string(shaderConfig));
		shaderFolder += "/";

		// init shader
		CShader* shader = new CShader();

		if (buildShader(shader, xmlReader, shaderConfig, shaderFolder.c_str(), false) == false)
		{
			std::string name = shader->getName();
			delete shader;
			shader = NULL;

			// try test loaded shader
			shader = getShaderByName(name.c_str());
		}

		xmlReader->drop();
		return shader;
	}

	bool CShaderManager::buildShader(CShader* shader, io::IXMLReader* xmlReader, const char* source, const char* shaderFolder, bool rebuild)
	{
		// init shader config
		shader->initShader(xmlReader, source, shaderFolder);

		CShader* instancingShader = shader->getInstancingShader();

		// init instancing batching
		IShaderInstancing* instancing = NULL;
		std::string instancingVertex = shader->getInstancingVertex();
		if (!instancingVertex.empty())
		{
			if (instancingVertex == "standard_color")
				instancing = new CStandardColorInstancing();
			else if (instancingVertex == "standard_sg")
			{
				instancing = new CStandardSGInstancing();

				if (instancingShader)
				{
					instancingShader->setShadowDepthWriteShader("SDWStandardSGInstancing");
					instancingShader->setShadowDistanceWriteShader("SDWDistanceStandardSGInstancing");
				}
			}
			else if (instancingVertex == "tangents_sg")
			{
				instancing = new CTBNSGInstancing();

				if (instancingShader)
				{
					instancingShader->setShadowDepthWriteShader("SDWTangentSGInstancing");
					instancingShader->setShadowDistanceWriteShader("SDWDistanceTangentSGInstancing");
				}
			}
		}
		shader->setInstancing(instancing);

		const std::string& shaderName = shader->getName();

		// warning if it not yet support instancing
		if (shader->isSupportInstancing() && instancing == NULL)
		{
			char log[512];
			sprintf(log, "!!! Warning: Name '%s' have support instancing, but xml file missing 'vertexType'", shaderName.c_str());
			os::Printer::log(log);
		}

		if (!rebuild)
		{
			// if this name it loaded
			if (m_listShaderID.find(shaderName) != m_listShaderID.end())
			{
				char log[512];
				sprintf(log, "!!! Warning: Name '%s' is loaded <-- SKIP", shaderName.c_str());
				os::Printer::log(log);

				return false;
			}
		}

		// build shader
		shader->buildShader();
		shader->buildUIUniform();

		// if shader load success
		int materialID = shader->getMaterialRenderID();

		if (shaderName.empty() == false && materialID >= 0)
		{
			m_listShaderID[shaderName] = materialID;

			if (!rebuild)
				m_listShader.push_back(shader);
		}
		else
		{
			char log[512];
			sprintf(log, "Load shader error: %s !!!!!", shaderName.c_str());
			os::Printer::log(log);

			return false;
		}

		std::vector<std::string>& deps = shader->getDependents();
		for (std::string& p : deps)
		{
			if (loadShader(p.c_str()) == NULL)
			{
				char log[512];
				sprintf(log, "!!! Warning: Name '%s' fail dependent:'%s'", shaderName.c_str(), p.c_str());
				os::Printer::log(log);
			}
		}

		return true;
	}

	bool CShaderManager::rebuildShader(CShader* shader)
	{
		std::string source = shader->getSource();
		const char* shaderConfig = source.c_str();

		char log[512];

		io::IXMLReader* xmlReader = getIrrlichtDevice()->getFileSystem()->createXMLReader(shaderConfig);
		if (xmlReader == NULL)
		{
			sprintf(log, "Reload shader: %s - File not found", shaderConfig);
			os::Printer::log(log);
			return NULL;
		}

		sprintf(log, "Reload shader: %s", shaderConfig);
		os::Printer::log(log);

		std::string shaderFolder = CPath::getFolderPath(std::string(shaderConfig));
		shaderFolder += "/";

		buildShader(shader, xmlReader, shaderConfig, shaderFolder.c_str(), true);

		xmlReader->drop();

		return true;
	}

	int CShaderManager::getShaderIDByName(const char* name)
	{
		std::map<std::string, int>::iterator it = m_listShaderID.find(name);
		if (it != m_listShaderID.end())
			return (*it).second;

		char log[128];
		sprintf(log, "Can not get shader: %s", name);
		os::Printer::log(log);
		return 0;
	}

	CShader* CShaderManager::getShaderByName(const char* name)
	{
		for (u32 i = 0, n = (u32)m_listShader.size(); i < n; i++)
		{
			if (m_listShader[i]->getName() == name)
				return m_listShader[i];
		}

		return NULL;
	}

	CShader* CShaderManager::getShaderByPath(const char* path)
	{
		for (u32 i = 0, n = (u32)m_listShader.size(); i < n; i++)
		{
			if (m_listShader[i]->getSource() == path)
				return m_listShader[i];
		}

		return NULL;
	}

	CShader* CShaderManager::getShaderByID(int id)
	{
		for (u32 i = 0, n = (u32)m_listShader.size(); i < n; i++)
		{
			if (m_listShader[i]->getMaterialRenderID() == id)
				return m_listShader[i];
		}

		return NULL;
	}

}