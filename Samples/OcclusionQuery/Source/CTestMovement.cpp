#include "pch.h"
#include "CTestMovement.h"

#include "GameObject/CGameObject.h"
#include "Tween/CTweenVector3df.h"
#include "Tween/CTweenManager.h"

CTestMovement::CTestMovement() :
	m_tween(NULL),
	m_cycle(1),
	m_duration(3000.0f),
	m_from(0.0f, 2.0f, 0.0f),
	m_to(0.0f, -2.0f, 0.0f)
{

}

CTestMovement::~CTestMovement()
{
	if (m_tween)
	{
		m_tween->stop();
		m_tween = NULL;
	}
}

void CTestMovement::initComponent()
{
	startCycle();
}

void CTestMovement::startCycle()
{
	if (m_cycle % 2 == 0)
		m_tween = new CTweenVector3df(m_from, m_to, m_duration);
	else
		m_tween = new CTweenVector3df(m_to, m_from, m_duration);

	m_cycle++;

	m_tween->setEase(EEasingFunctions::EaseInOutQuad);
	m_tween->setEndDelay(1000.0f);
	m_tween->OnUpdate = [&](CTween* t)
		{
			if (t == m_tween)
				m_gameObject->getTransformEuler()->setPosition(m_tween->getValue());
		};
	m_tween->OnFinish = [&](CTween* t)
		{
			if (t == m_tween)
				startCycle();
		};

	CTweenManager::getInstance()->addTween(m_tween);
}

void CTestMovement::updateComponent()
{

}