#include "pch.h"
#include "SkylichtEngine.h"
#include "CLuckyDrawConfig.h"

CLuckyDrawConfig::CLuckyDrawConfig()
{

}

CLuckyDrawConfig::~CLuckyDrawConfig()
{
	for (SStateConfig *cfg : m_configs)
		delete cfg;

	m_configs.clear();
}

void CLuckyDrawConfig::initConfigs()
{
	SStateConfig *cfg = new SStateConfig();
	cfg->BackgroundImage = "LuckyDraw/state_01.png";

	m_configs.push_back(cfg);
}