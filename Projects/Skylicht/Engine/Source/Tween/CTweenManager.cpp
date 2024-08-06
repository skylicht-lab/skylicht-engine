#include "pch.h"
#include "CTweenManager.h"

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CTweenManager);

	CTweenManager::CTweenManager()
	{

	}

	CTweenManager::~CTweenManager()
	{

	}

	void CTweenManager::update()
	{
		m_tweens.insert(m_tweens.end(), m_insert.begin(), m_insert.end());
		m_insert.clear();

		for (CTween* tween : m_tweens)
		{
			bool isRemoved = false;
			for (CTween* t : m_remove)
			{
				if (t == tween)
				{
					isRemoved = true;
					break;
				}
			}

			if (!isRemoved)
				tween->update();
		}

		for (CTween* tween : m_remove)
		{
			std::vector<CTween*>::iterator i = std::find(m_tweens.begin(), m_tweens.end(), tween);
			if (i != m_tweens.end())
			{
				m_tweens.erase(i);
			}
			delete tween;
		}
		m_remove.clear();
	}

	void CTweenManager::addTween(CTween* tween)
	{
		for (CTween* t : m_insert)
		{
			if (t == tween)
				return;
		}

		m_insert.push_back(tween);
	}

	void CTweenManager::removeTween(CTween* tween)
	{
		for (CTween* t : m_remove)
		{
			if (t == tween)
				return;
		}

		m_remove.push_back(tween);
	}
}