#pragma once 

#include "CTween.h"

namespace Skylicht
{
	class CTweenQuaternion : public CTween
	{
	protected:
		core::quaternion m_value;
		core::quaternion m_begin;
		core::quaternion m_end;
	public:
		CTweenQuaternion(const core::quaternion& begin, const core::quaternion& end, float duration);

		virtual ~CTweenQuaternion();

		virtual void updateValue();

		void setBegin(const core::quaternion& begin);

		void setEnd(const core::quaternion& end);

		const core::quaternion& getValue()
		{
			return m_value;
		}
	};
}