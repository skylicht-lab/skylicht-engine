#include "pch.h"
#include "CTweenVector3df.h"

namespace Skylicht
{
	CTweenVector3df::CTweenVector3df(const core::vector3df& begin, const core::vector3df& end, float duration)
	{
		setBegin(begin);
		setEnd(end);
		setDuration(duration);
	}

	CTweenVector3df::~CTweenVector3df()
	{

	}

	void CTweenVector3df::updateValue()
	{
		m_value.X = getValueByIndex(0);
		m_value.Y = getValueByIndex(1);
		m_value.Z = getValueByIndex(2);
	}

	void CTweenVector3df::setBegin(const core::vector3df& begin)
	{
		setBeginValue(0, begin.X);
		setBeginValue(1, begin.Y);
		setBeginValue(2, begin.Z);
	}

	void CTweenVector3df::setEnd(const core::vector3df& end)
	{
		setEndValue(0, end.X);
		setEndValue(1, end.Y);
		setEndValue(2, end.Z);
	}
}