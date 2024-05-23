#include "Components/CComponentSystem.h"
#include "Tween/CTweenVector3df.h"

class CTestMovement : public CComponentSystem
{
protected:
	CTweenVector3df* m_tween;

	int m_cycle;
	float m_duration;
	core::vector3df m_from;
	core::vector3df m_to;

public:
	CTestMovement();

	virtual ~CTestMovement();

	virtual void initComponent();

	virtual void updateComponent();

protected:

	void startCycle();
};