#pragma once 

#include "CTween.h"

namespace Skylicht
{
	class SKYLICHT_API CTweenVector2df : public CTween
	{
	protected:
		core::vector2df m_value;

	public:
		CTweenVector2df(const core::vector2df& begin, const core::vector2df& end, float duration);

		virtual ~CTweenVector2df();

		virtual void updateValue();

		void setBegin(const core::vector2df& begin);

		void setEnd(const core::vector2df& end);

		const core::vector2df& getValue()
		{
			return m_value;
		}
	};
}