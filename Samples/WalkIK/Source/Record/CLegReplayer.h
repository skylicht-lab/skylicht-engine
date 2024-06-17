#pragma once

#include "CRecorder.h"
#include "LegController/CLegIK.h"
#include "RenderMesh/CRenderMesh.h"
#include "Components/CComponentSystem.h"
#include "Components/ILateUpdate.h"

using namespace Skylicht;

class CLegReplayer :
	public CComponentSystem,
	public ILateUpdate
{
protected:
	bool m_drawDebug;
	float m_frame;
	bool m_pause;
	bool m_rootMotion;
	float m_frameFrom;
	float m_frameTo;
	float m_duration;

	CRenderMesh* m_renderMesh;
	CRecorder* m_recoder;
	CWorldTransformData* m_root;
	std::vector<CLegIK*> m_legs;

public:
	CLegReplayer();

	virtual ~CLegReplayer();

	virtual void initComponent();

	virtual void updateComponent();

	void setRecorder(CRecorder* recoder);

	CLegIK* addLeg(CWorldTransformData* root, CWorldTransformData* leg);

	inline CWorldTransformData* getRoot()
	{
		return m_root;
	}

	inline std::vector<CLegIK*>& getLegs()
	{
		return m_legs;
	}

	inline void setRootMotion(bool b)
	{
		m_rootMotion = b;
	}

	inline bool isRootMotion()
	{
		return m_rootMotion;
	}

	virtual void lateUpdate();
};