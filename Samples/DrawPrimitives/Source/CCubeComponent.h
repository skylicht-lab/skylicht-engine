#pragma once

class CCubeComponent : public CComponentSystem
{
public:
	CCubeComponent();

	virtual ~CCubeComponent();

	virtual void initComponent();

	virtual void updateComponent();
};