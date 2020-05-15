#pragma once

class CLuckyDrawConfig : public CGameSingleton<CLuckyDrawConfig>
{
public:
	struct SStateConfig
	{
		std::string BackgroundImage;
	};

protected:
	std::vector<SStateConfig*> m_configs;

public:
	CLuckyDrawConfig();

	virtual ~CLuckyDrawConfig();

	void initConfigs();

	int getNumberOfConfig()
	{
		return (int)m_configs.size();
	}

	SStateConfig* getConfig(int i)
	{
		return m_configs[i];
	}
};