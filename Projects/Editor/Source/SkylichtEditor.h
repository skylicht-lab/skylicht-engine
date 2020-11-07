#pragma once

#include "IApplicationEventReceiver.h"

#include "GUI/CGUIContext.h"
#include "Editor/CEditor.h"

class SkylichtEditor : public IApplicationEventReceiver
{
public:
	enum EEditorState
	{
		Startup = 0,
		InitGUI,
		InitEngine,
		Loading,
		Running,
	};

private:
	EEditorState m_editorState;

	Editor::CEditor *m_editor;

public:
	SkylichtEditor();
	virtual ~SkylichtEditor();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onResize(int w, int h);

	virtual void onInitApp();

	virtual void onQuitApp();
};