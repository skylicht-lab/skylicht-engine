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
		float ts = getTimeStep();

		// delay call
		m_delayCalls.insert(m_delayCalls.end(), m_insertCalls.begin(), m_insertCalls.end());
		m_insertCalls.clear();

		for (SDelayCall* delayCall : m_delayCalls)
		{
			delayCall->Time = delayCall->Time - ts;
			if (delayCall->Time <= 0.0f)
			{
				delayCall->Function();
				m_removeCalls.push_back(delayCall);
			}
		}

		for (SDelayCall* delayCall : m_removeCalls)
		{
			std::vector<SDelayCall*>::iterator i = std::find(m_delayCalls.begin(), m_delayCalls.end(), delayCall);
			if (i != m_delayCalls.end())
				m_delayCalls.erase(i);
			delete delayCall;
		}
		m_removeCalls.clear();


		// tween
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
				m_tweens.erase(i);
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

	void CTweenManager::addDelayCall(float time, std::function<void()> function)
	{
		m_insertCalls.push_back(new SDelayCall());

		SDelayCall* delayCall = m_insertCalls.back();
		delayCall->Time = time;
		delayCall->Function = function;
	}
}