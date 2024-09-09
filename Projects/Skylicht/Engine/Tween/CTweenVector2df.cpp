#include "pch.h"
#include "CTweenVector2df.h"

namespace Skylicht
{
	CTweenVector2df::CTweenVector2df(const core::vector2df& begin, const core::vector2df& end, float duration)
	{
		setBegin(begin);
		setEnd(end);
		setDuration(duration);
	}

	CTweenVector2df::~CTweenVector2df()
	{

	}

	void CTweenVector2df::updateValue()
	{
		m_value.X = getValueByIndex(0);
		m_value.Y = getValueByIndex(1);
	}

	void CTweenVector2df::setBegin(const core::vector2df& begin)
	{
		setBeginValue(0, begin.X);
		setBeginValue(1, begin.Y);
	}

	void CTweenVector2df::setEnd(const core::vector2df& end)
	{
		setEndValue(0, end.X);
		setEndValue(1, end.Y);
	}
}