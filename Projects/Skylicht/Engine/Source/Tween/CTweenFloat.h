#pragma once 

#include "CTween.h"

namespace Skylicht
{
	class CTweenFloat : public CTween
	{
	public:
		CTweenFloat(float begin, float end, float duration);

		virtual ~CTweenFloat();

		void setBegin(float begin);

		void setEnd(float end);

		float getValue();
	};
}