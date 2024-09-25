#pragma once

#include "ViewManager/CView.h"

#include "AudioStream/CZipAudioStreamFactory.h"
#include "AudioStream/CZipAudioFileStream.h"
#include "Graphics2D/SpriteFrame/CSpriteAtlas.h"

class CViewDemo : public CView
{
protected:
	CZipAudioStreamFactory* m_streamFactory;

	Audio::CAudioEmitter* m_music;
	Audio::CAudioEmitter* m_sound;

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	void onGUI();
};