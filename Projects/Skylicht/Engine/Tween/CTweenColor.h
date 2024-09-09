#pragma once 

#include "CTween.h"

namespace Skylicht
{
	class SKYLICHT_API CTweenColor : public CTween
	{
	protected:
		SColor m_value;

	public:
		CTweenColor(const SColor& begin, const SColor& end, float duration);

		virtual ~CTweenColor();

		virtual void updateValue();

		void setBegin(const SColor& begin);

		void setEnd(const SColor& end);

		const SColor& getValue()
		{
			return m_value;
		}
	};
}