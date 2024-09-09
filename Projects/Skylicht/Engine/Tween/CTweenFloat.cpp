#include "pch.h"
#include "CTweenFloat.h"

namespace Skylicht
{
	CTweenFloat::CTweenFloat(float begin, float end, float duration)
	{
		setBegin(begin);
		setEnd(end);
		setDuration(duration);
	}

	CTweenFloat::~CTweenFloat()
	{

	}

	void CTweenFloat::setBegin(float begin)
	{
		setBeginValue(0, begin);
	}

	void CTweenFloat::setEnd(float end)
	{
		setEndValue(0, end);
	}

	float CTweenFloat::getValue()
	{
		return getValueByIndex(0);
	}

	void CTweenFloat::updateValue()
	{
		// do nothing
	}
}