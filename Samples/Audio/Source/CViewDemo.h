#pragma once

#include "ViewManager/CView.h"

#include "AudioStream/CZipAudioStreamFactory.h"
#include "AudioStream/CZipAudioFileStream.h"

class CViewDemo : public CView
{
protected:
	CZipAudioStreamFactory *m_streamFactory;

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();
};