#pragma once

#include <SpriteDraw/CSprite.h>
#include <Graphics2D/SpriteFrame/CSpriteAtlas.h>

#include "SkylichtAudio.h"

class CTestSoundComponent : public Skylicht::CSprite
{
protected:
	CSpriteAtlas* m_sprite;
	SFrame* m_frame;
	Audio::CAudioEmitter* m_sound;

	float m_angle;
	float m_radius;

public:
	CTestSoundComponent();

	virtual ~CTestSoundComponent();

	virtual void initComponent();

	virtual void updateComponent();

	void setFrame(CSpriteAtlas* sprite, SFrame* frame);

	inline void setAudioEmitter(Audio::CAudioEmitter* sound)
	{
		m_sound = sound;
	}
};