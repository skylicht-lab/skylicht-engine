#include "pch.h"
#include "CTweenColor.h"

namespace Skylicht
{
	CTweenColor::CTweenColor(const SColor& begin, const SColor& end, float duration)
	{
		setBegin(begin);
		setEnd(end);
		setDuration(duration);
	}

	CTweenColor::~CTweenColor()
	{

	}

	void CTweenColor::setBegin(const SColor& begin)
	{
		setBeginValue(0, (float)begin.getRed());
		setBeginValue(1, (float)begin.getGreen());
		setBeginValue(2, (float)begin.getBlue());
		setBeginValue(3, (float)begin.getAlpha());
	}

	void CTweenColor::setEnd(const SColor& end)
	{
		setEndValue(0, (float)end.getRed());
		setEndValue(1, (float)end.getGreen());
		setEndValue(2, (float)end.getBlue());
		setEndValue(3, (float)end.getAlpha());
	}

	void CTweenColor::updateValue()
	{
		m_value.setRed((u32)getValueByIndex(0));
		m_value.setGreen((u32)getValueByIndex(1));
		m_value.setBlue((u32)getValueByIndex(2));
		m_value.setAlpha((u32)getValueByIndex(3));
	}
}