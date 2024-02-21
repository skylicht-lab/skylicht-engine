#include "pch.h"
#include "CRigidbody.h"
#include "GameObject/CGameObject.h"

namespace Skylicht
{
	namespace Physics
	{
		ACTIVATOR_REGISTER(CRigidbody);

		CATEGORY_COMPONENT(CRigidbody, "Rigidbody", "Physics");

		CRigidbody::CRigidbody()
		{

		}

		CRigidbody::~CRigidbody()
		{

		}

		void CRigidbody::initComponent()
		{
			// convert to readonly transform
			m_gameObject->setupMatrixTransform();
			m_gameObject->getTransformMatrix()->setIsWorldTransform(true);
		}

		void CRigidbody::updateComponent()
		{
			CTransformMatrix* transform = m_gameObject->getTransformMatrix();
			if (transform == NULL)
				return;

		}
	}
}