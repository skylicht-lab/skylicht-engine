#pragma once 

#include "CTween.h"

namespace Skylicht
{
	class SKYLICHT_API CTweenVector3df : public CTween
	{
	protected:
		core::vector3df m_value;

	public:
		CTweenVector3df(const core::vector3df& begin, const core::vector3df& end, float duration);

		virtual ~CTweenVector3df();

		virtual void updateValue();

		void setBegin(const core::vector3df& begin);

		void setEnd(const core::vector3df& end);

		const core::vector3df& getValue()
		{
			return m_value;
		}
	};
}