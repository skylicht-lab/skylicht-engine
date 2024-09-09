#include "pch.h"
#include "Utils/CPath.h"
#include "Importer/IAnimationImporter.h"
#include "Importer/Collada/CColladaAnimLoader.h"
#include "Importer/FBX/CFBXAnimLoader.h"
#include "Importer/Skylicht/CSkylichtAnimLoader.h"
#include "Exporter/Skylicht/CSkylichtAnimExporter.h"
#include "CAnimationManager.h"

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CAnimationManager);

	CAnimationManager::CAnimationManager()
	{

	}

	CAnimationManager::~CAnimationManager()
	{
		releaseAllClips();
		releaseAllAnimations();
	}

	CAnimation* CAnimationManager::createAnimation(const char* name)
	{
		std::map<std::string, CAnimation*>::iterator findCache = m_animations.find(name);
		if (findCache != m_animations.end())
		{
			return (*findCache).second;
		}

		CAnimation* anim = new CAnimation();
		m_animations[name] = anim;
		return anim;
	}

	CAnimationClip* CAnimationManager::loadAnimation(const char* resource)
	{
		IAnimationImporter* importer = NULL;
		CAnimationClip* output = NULL;

		// load from file
		std::string ext = CPath::getFileNameExt(resource);
		if (ext == "dae")
			importer = new CColladaAnimLoader();
		else if (ext == "fbx")
			importer = new CFBXAnimLoader();
		else if (ext == "sanim")
			importer = new CSkylichtAnimLoader();

		if (importer != NULL)
		{
			output = loadAnimation(resource, importer);
			delete importer;
		}

		return output;
	}

	CAnimationClip* CAnimationManager::loadAnimation(const char* resource, IAnimationImporter* importer)
	{
		// find in cached
		std::map<std::string, CAnimationClip*>::iterator findCache = m_clips.find(resource);
		if (findCache != m_clips.end())
		{
			return (*findCache).second;
		}

		CAnimationClip* output = NULL;

		if (importer != NULL)
		{
			output = new CAnimationClip();

			char log[512];

			// load animation
			if (importer->loadAnimation(resource, output) == true)
			{
				sprintf(log, "Load animation: %s", resource);
				os::Printer::log(log);

				// cached
				m_clips[resource] = output;
			}
			else
			{
				sprintf(log, "Load animation: %s failed", resource);
				os::Printer::log(log);

				// load failed!
				delete output;
				output = NULL;
			}
		}

		return output;
	}

	bool CAnimationManager::exportAnimation(CAnimationClip* clip, const char* output)
	{
		IAnimationExporter* exporter = NULL;

		std::string ext = CPath::getFileNameExt(output);
		if (ext == "sanim")
			exporter = new CSkylichtAnimExporter();

		if (exporter != NULL)
		{
			bool result = exporter->exportAnim(clip, output);
			delete exporter;
			return result;
		}

		return false;
	}

	bool CAnimationManager::exportAnimation(CAnimationClip* clip, const char* output, IAnimationExporter* exporter)
	{
		if (exporter != NULL)
			return exporter->exportAnim(clip, output);
		return false;
	}

	void CAnimationManager::releaseAllClips()
	{
		std::map<std::string, CAnimationClip*>::iterator i = m_clips.begin(), end = m_clips.end();
		while (i != end)
		{
			delete (*i).second;
			++i;
		}

		m_clips.clear();
	}

	void CAnimationManager::releaseAllAnimations()
	{
		std::map<std::string, CAnimation*>::iterator i = m_animations.begin(), end = m_animations.end();
		while (i != end)
		{
			delete (*i).second;
			++i;
		}

		m_animations.clear();
	}
}