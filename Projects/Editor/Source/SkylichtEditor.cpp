#include "pch.h"
#include "SkylichtEngine.h"
#include "SkylichtEditor.h"

#include "Utils/CColor.h"

#include "AssetManager/CAssetManager.h"

void installApplication(const std::vector<std::string>& argv)
{
	SkylichtEditor* app = new SkylichtEditor();
	getApplication()->registerAppEvent("SkylichtEditor", app);
}

SkylichtEditor::SkylichtEditor() :
	m_editor(NULL),
	m_editorState(Startup)
{
	Editor::CAssetManager::createGetInstance();
}

SkylichtEditor::~SkylichtEditor()
{
	Editor::CAssetManager::releaseInstance();

	Editor::GUI::CGUIContext::destroyGUI();

	if (m_editor != NULL)
		delete m_editor;
}

void SkylichtEditor::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();
	app->setClearColor(CColor::toSRGB(SColor(255, 56, 56, 56)));
	app->showFPS(false);
}

void SkylichtEditor::onUpdate()
{
	float currentTime = (float)getIrrlichtDevice()->getTimer()->getTime();

	switch (m_editorState)
	{
	case Startup:
		// init engine
		m_editorState = InitGUI;
		break;
	case InitGUI:
	{
		// import resources
		CBaseApp* app = getApplication();
		app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);
		app->getFileSystem()->addFileArchive(app->getBuiltInPath("Editor.zip"), false, false);

		// load gui shader
		CShaderManager* shaderMgr = CShaderManager::getInstance();
		shaderMgr->initGUIShader();

		// init editor gui		
		u32 w = app->getWidth();
		u32 h = app->getHeight();
		Editor::GUI::CGUIContext::initGUI((float)w, (float)h);

		m_editor = new Editor::CEditor();
		m_editorState = InitEngine;
	}
	break;
	case InitEngine:
	{
		CShaderManager::getInstance()->initBasicShader();
		CShaderManager::getInstance()->initSGDeferredShader();

		// import project		
		m_editor->initImportGUI();

		// change state loading
		m_editorState = Loading;
	}
	break;
	case Loading:
	{
		// loading project
		if (m_editor->isImportFinish() == true)
		{
			m_editor->closeImportDialog();
			m_editor->initEditorGUI();
			m_editorState = Running;
		}

		// running
		m_editor->update();
		Editor::GUI::CGUIContext::update(currentTime);
	}
	break;
	default:
		// running
		m_editor->update();
		Editor::GUI::CGUIContext::update(currentTime);
		break;
	}
}

void SkylichtEditor::onRender()
{
	Editor::GUI::CGUIContext::render();
}

void SkylichtEditor::onPostRender()
{
	// post render application
}

bool SkylichtEditor::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SkylichtEditor::onResize(int w, int h)
{
	// on window size changed
	Editor::GUI::CGUIContext::resize((float)w, (float)h);
}

void SkylichtEditor::onResume()
{
	// resume application
}

void SkylichtEditor::onPause()
{
	// pause application
}

void SkylichtEditor::onQuitApp()
{
	// end application
	delete this;
}