#include "pch.h"
#include "CTweenQuaternion.h"

namespace Skylicht
{
	CTweenQuaternion::CTweenQuaternion(const core::quaternion& begin, const core::quaternion& end, float duration)
	{
		setBegin(begin);
		setEnd(end);
		setDuration(duration);
	}

	CTweenQuaternion::~CTweenQuaternion()
	{

	}

	void CTweenQuaternion::updateValue()
	{
		float time = m_percentValue;
		m_value.slerp(m_begin, m_end, time);
	}

	void CTweenQuaternion::setBegin(const core::quaternion& begin)
	{
		m_begin = begin;
	}

	void CTweenQuaternion::setEnd(const core::quaternion& end)
	{
		m_end = end;
	}
}