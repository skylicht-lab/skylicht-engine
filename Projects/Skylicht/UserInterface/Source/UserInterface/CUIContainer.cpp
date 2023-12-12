#include "pch.h"
#include "CUIContainer.h"

namespace Skylicht
{
	namespace UI
	{
		CUIConatiner::CUIConatiner() :
			m_enable(true)
		{
			CEventManager::getInstance()->registerProcessorEvent(m_name, this);
		}

		CUIConatiner::~CUIConatiner()
		{
			CEventManager::getInstance()->unRegisterProcessorEvent(this);
		}

		bool CUIConatiner::OnProcessEvent(const SEvent& event)
		{
			if (!m_enable)
				return false;

			bool skipAnotherEvent = false;


			return skipAnotherEvent;
		}
	}
}