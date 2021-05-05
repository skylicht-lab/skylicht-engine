#pragma once 

#include "CTween.h"

namespace Skylicht
{
	class CTweenMatrix4 : public CTween
	{
	protected:
		core::matrix4 m_value;

	public:
		CTweenMatrix4(const core::matrix4& begin, const core::matrix4& end, float duration);

		virtual ~CTweenMatrix4();

		virtual void updateValue();

		void setBegin(const core::matrix4& begin);

		void setEnd(const core::matrix4& end);

		const core::matrix4& getValue()
		{
			return m_value;
		}
	};
}