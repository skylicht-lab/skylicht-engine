// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#include "pch.h"
#include "CShaderManager.h"
#include "CShader.h"
#include "Utils/CPath.h"

namespace Skylicht
{

	CShaderManager::CShaderManager()
	{
		m_currentRenderID = -1;
		m_currentMeshBuffer = NULL;
		m_currentMatRendering = NULL;
		m_haveBakeLMShader = false;
	}

	CShaderManager::~CShaderManager()
	{
		releaseAll();
	}

	void CShaderManager::releaseAll()
	{
		for (int i = 0, n = (int)m_listShader.size(); i < n; i++)
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
#ifndef LINUX_SERVER
#endif
	}

	void CShaderManager::initSkylichtEngineShader()
	{
#ifndef LINUX_SERVER		
#endif
	}


	void CShaderManager::initShader()
	{
#ifndef LINUX_SERVER
		// todo init
		initBasicShader();

		initSkylichtEngineShader();
#endif
	}

	CShader* CShaderManager::loadShader(const char *shaderConfig, IShaderCallback *callback, bool releaseCallback)
	{
#ifdef LINUX_SERVER
		return NULL;		
#else
		std::string shaderFolder = CPath::getFolderPath(std::string(shaderConfig));
		shaderFolder += "/";

		io::IXMLReader *xmlReader = getIrrlichtDevice()->getFileSystem()->createXMLReader(shaderConfig);
		if (xmlReader == NULL)
			return NULL;

		char log[512];
		sprintf(log, "Load shader: %s", shaderConfig);
		os::Printer::log(log);

		// init shader
		CShader *shader = new CShader();
		shader->initShader(xmlReader, shaderFolder.c_str());
		shader->setShaderPath(shaderConfig);
		xmlReader->drop();

		// apply callback
		shader->setCallback(callback, releaseCallback);

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
#endif
	}

	// getShaderIDByName
	// todo:
	// get shader id by name
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

	// getShaderByName
	CShader* CShaderManager::getShaderByName(const char *name)
	{
		for (int i = 0, n = (int)m_listShader.size(); i < n; i++)
		{
			if (m_listShader[i]->getName() == name)
				return m_listShader[i];
		}

		return NULL;
	}

	// getShaderByPath
	CShader* CShaderManager::getShaderByPath(const char *path)
	{
		for (int i = 0, n = (int)m_listShader.size(); i < n; i++)
		{
			if (m_listShader[i]->getShaderPath() == path)
				return m_listShader[i];
		}

		return NULL;
	}

	CShader* CShaderManager::getShaderByID(int id)
	{
		for (int i = 0, n = (int)m_listShader.size(); i < n; i++)
		{
			if (m_listShader[i]->getMaterialRenderID() == id)
				return m_listShader[i];
		}

		return NULL;
	}

} // namespace
