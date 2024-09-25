#pragma once

#include <SpriteDraw/CSprite.h>
#include <Graphics2D/SpriteFrame/CSpriteAtlas.h>

#include "SkylichtAudio.h"

class CTestSoundComponent : public Skylicht::CSprite
{
protected:
	Audio::CAudioEmitter* m_sound;

	float m_angle;
	float m_radius;

public:
	CTestSoundComponent();

	virtual ~CTestSoundComponent();

	virtual void initComponent();

	virtual void updateComponent();

	inline void setAudioEmitter(Audio::CAudioEmitter* sound)
	{
		m_sound = sound;
	}
};