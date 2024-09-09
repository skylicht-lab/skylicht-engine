#include "pch.h"
#include "CTweenMatrix4.h"

namespace Skylicht
{
	CTweenMatrix4::CTweenMatrix4(const core::matrix4& begin, const core::matrix4& end, float duration)
	{
		setBegin(begin);
		setEnd(end);
		setDuration(duration);
	}

	CTweenMatrix4::~CTweenMatrix4()
	{

	}

	void CTweenMatrix4::updateValue()
	{
		float* p = m_value.pointer();
		for (int i = 0; i < 16; i++)
			p[i] = getValueByIndex(i);
	}

	void CTweenMatrix4::setBegin(const core::matrix4& begin)
	{
		float* p = m_value.pointer();
		for (int i = 0; i < 16; i++)
			setBeginValue(i, p[i]);
	}

	void CTweenMatrix4::setEnd(const core::matrix4& end)
	{
		float* p = m_value.pointer();
		for (int i = 0; i < 16; i++)
			setEndValue(i, p[i]);
	}
}