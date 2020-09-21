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

namespace Skylicht
{

	CShaderManager::CShaderManager() :
		m_currentRenderID(-1),
		m_currentMeshBuffer(NULL),
		m_currentMatRendering(NULL),
		BoneMatrix(NULL)
	{
	}

	CShaderManager::~CShaderManager()
	{
		releaseAll();
	}

	void CShaderManager::releaseAll()
	{
		for (u32 i = 0, n = m_listShader.size(); i < n; i++)
			m_listShader[i]->drop();

		m_listShader.clear();
	}

	std::string CShaderManager::getShaderFileName(const char *fileName)
	{
		std::string ret = fileName;

		IVideoDriver *driver = getVideoDriver();
		if (driver->getDriverType() == video::EDT_DIRECT3D11)
			ret += std::string(".hlsl");
		else
			ret += std::string(".glsl");

		return ret;
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

		loadShader("BuiltIn/Shader/Basic/AlphaTest.xml");
		loadShader("BuiltIn/Shader/Basic/AlphaBlend.xml");

		loadShader("BuiltIn/Shader/Basic/Skin.xml");

		loadShader("BuiltIn/Shader/ShadowMap/ShadowDepthWrite.xml");
		loadShader("BuiltIn/Shader/ShadowMap/ShadowCubeDepthWrite.xml");

		loadShader("BuiltIn/Shader/Basic/TextureSRGB.xml");
		loadShader("BuiltIn/Shader/Basic/TextureLinearRGB.xml");
		loadShader("BuiltIn/Shader/Basic/Luminance.xml");

		loadShader("BuiltIn/Shader/Lightmap/Lightmap.xml");
		loadShader("BuiltIn/Shader/Lightmap/LightmapUV.xml");
		loadShader("BuiltIn/Shader/Lightmap/IndirectTest.xml");
		loadShader("BuiltIn/Shader/Lightmap/LightmapVertex.xml");

		loadShader("BuiltIn/Shader/PostProcessing/AdaptLuminance.xml");
		loadShader("BuiltIn/Shader/PostProcessing/PostEffect.xml");

		loadShader("BuiltIn/Shader/Particle/ParticleBillboardAdditive.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleBillboardTransparent.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleVelocityAdditive.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleVelocityTransparent.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleOrientationAdditive.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleOrientationTransparent.xml");
		loadShader("BuiltIn/Shader/Particle/ParticleTrailTurbulenceAdditive.xml");
	}

	void CShaderManager::initSGDeferredShader()
	{
		loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Color.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/DiffuseNormal.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Specular.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/Diffuse.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/SpecularGlossiness.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Deferred/SpecularGlossinessMask.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGDirectionalLight.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGDirectionalLightBake.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGPointLight.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Lighting/SGPointLightShadow.xml");
		loadShader("BuiltIn/Shader/SpecularGlossiness/Forward/SH.xml");
	}

	void CShaderManager::initSkylichtEngineShader()
	{
	}


	void CShaderManager::initShader()
	{
		initBasicShader();

		initSkylichtEngineShader();
	}

	CShader* CShaderManager::loadShader(const char *shaderConfig)
	{
		std::string shaderFolder = CPath::getFolderPath(std::string(shaderConfig));
		shaderFolder += "/";

		char log[512];

		io::IXMLReader *xmlReader = getIrrlichtDevice()->getFileSystem()->createXMLReader(shaderConfig);
		if (xmlReader == NULL)
		{
			sprintf(log, "Load shader: %s - File not found", shaderConfig);
			os::Printer::log(log);

			return NULL;
		}

		sprintf(log, "Load shader: %s", shaderConfig);
		os::Printer::log(log);

		// init shader
		CShader *shader = new CShader();
		shader->initShader(xmlReader, shaderFolder.c_str());
		shader->setShaderPath(shaderConfig);
		xmlReader->drop();

		// if shader load success
		const std::string& shaderName = shader->getName();
		int materialID = shader->getMaterialRenderID();

		if (shaderName.empty() == false && materialID >= 0)
		{
			m_listShaderID[shaderName] = materialID;
			m_listShader.push_back(shader);
		}
		else
		{
			char log[512];
			sprintf(log, "Load shader error: %s !!!!!", shaderName.c_str());
			os::Printer::log(log);

			delete shader;
			shader = NULL;
		}

		return shader;
	}

	int CShaderManager::getShaderIDByName(const char *name)
	{
		std::map<std::string, int>::iterator it = m_listShaderID.find(name);
		if (it != m_listShaderID.end())
			return (*it).second;

		char log[128];
		sprintf(log, "Can not get shader: %s", name);
		os::Printer::log(log);
		return 0;
	}

	CShader* CShaderManager::getShaderByName(const char *name)
	{
		for (u32 i = 0, n = m_listShader.size(); i < n; i++)
		{
			if (m_listShader[i]->getName() == name)
				return m_listShader[i];
		}

		return NULL;
	}

	CShader* CShaderManager::getShaderByPath(const char *path)
	{
		for (u32 i = 0, n = m_listShader.size(); i < n; i++)
		{
			if (m_listShader[i]->getShaderPath() == path)
				return m_listShader[i];
		}

		return NULL;
	}

	CShader* CShaderManager::getShaderByID(int id)
	{
		for (u32 i = 0, n = m_listShader.size(); i < n; i++)
		{
			if (m_listShader[i]->getMaterialRenderID() == id)
				return m_listShader[i];
		}

		return NULL;
	}

}