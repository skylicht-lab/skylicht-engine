#include "pch.h"
#include "CCamera.h"
#include "C3rdCamera.h"
#include "GameObject/CGameObject.h"
#include "Scene/CScene.h"

namespace Skylicht
{
	C3rdCamera::C3rdCamera()
	{

	}

	C3rdCamera::~C3rdCamera()
	{
		getGameObject()->getScene()->unRegisterEvent(this);
	}

	void C3rdCamera::initComponent()
	{
		getGameObject()->getScene()->registerEvent(getTypeName(), this);
		getGameObject()->setEnableEndUpdate(true);
	}

	void C3rdCamera::updateComponent()
	{

	}

	void C3rdCamera::endUpdate()
	{
		CTransformEuler* transform = m_gameObject->getTransformEuler();
		CCamera* camera = m_gameObject->getComponent<CCamera>();

		if (camera == NULL || transform == NULL)
			return;

		if (!camera->isInputReceiverEnabled())
			return;

		f32 timeDiff = getTimeStep();

		// skip lag
		const float delta = 1000.0f / 20.0f;
		if (timeDiff > delta)
			timeDiff = delta;

		// convert to sec
		timeDiff = timeDiff * 0.001f;

	}

	bool C3rdCamera::OnEvent(const SEvent& evt)
	{
		return false;
	}
}