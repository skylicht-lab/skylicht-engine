#pragma once 

#include "CTween.h"
#include "Utils/CGameSingleton.h"

namespace Skylicht
{
	class CTweenManager : public CGameSingleton<CTweenManager>
	{
	protected:
		std::vector<CTween*> m_tweens;
		std::vector<CTween*> m_insert;
		std::vector<CTween*> m_remove;
	public:
		CTweenManager();

		virtual ~CTweenManager();

		void update();

		void addTween(CTween* tween);

		void removeTween(CTween* tween);
	};
}