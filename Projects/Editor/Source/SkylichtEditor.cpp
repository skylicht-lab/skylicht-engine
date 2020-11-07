#include "pch.h"
#include "SkylichtEngine.h"
#include "SkylichtEditor.h"

#include "GridPlane/CGridPlane.h"
#include "Utils/CColor.h"

void installApplication(const std::vector<std::string>& argv)
{
	SkylichtEditor *app = new SkylichtEditor();
	getApplication()->registerAppEvent("SkylichtEditor", app);
}

SkylichtEditor::SkylichtEditor() :
	m_editor(NULL),
	m_editorState(Startup)
{

}

SkylichtEditor::~SkylichtEditor()
{
	if (m_editor != NULL)
		delete m_editor;

	Editor::GUI::CGUIContext::destroyGUI();
}

void SkylichtEditor::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();
	app->setClearColor(CColor::toSRGB(SColor(255, 56, 56, 56)));
	app->showFPS(false);

	/*
	CScene *m_scene;
	CCamera *m_guiCamera;
	CCamera *m_camera;
	CForwardRP *m_forwardRP;

	// create a Scene
	m_scene = new CScene();

	// create a Zone in Scene
	CZone *zone = m_scene->createZone();

	// create 2D camera
	CGameObject *guiCameraObject = zone->createEmptyObject();
	m_guiCamera = guiCameraObject->addComponent<CCamera>();
	m_guiCamera->setProjectionType(CCamera::OrthoUI);

	// create 3D camera
	CGameObject *camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);

	m_camera = camObj->getComponent<CCamera>();
	m_camera->setPosition(core::vector3df(0.0f, 1.5f, 4.0f));
	m_camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// 3d grid
	CGameObject *grid = zone->createEmptyObject();
	grid->addComponent<CGridPlane>();

	// lighting
	CGameObject *lightObj = zone->createEmptyObject();
	CDirectionalLight *directionalLight = lightObj->addComponent<CDirectionalLight>();
	SColor c(255, 255, 244, 214);
	directionalLight->setColor(SColorf(c));

	CTransformEuler *lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(0.0f, -1.5f, 2.0f);
	lightTransform->setOrientation(direction, CTransform::s_oy);

	// rendering pipe line

	m_forwardRP = new CForwardRP();
	m_forwardRP->initRender(w, h);
	*/
}

void SkylichtEditor::onUpdate()
{
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
		CShaderManager *shaderMgr = CShaderManager::getInstance();
		shaderMgr->initGUIShader();

		// init editor gui		
		u32 w = app->getWidth();
		u32 h = app->getHeight();
		Editor::GUI::CGUIContext::initGUI((float)w, (float)h);

		m_editor = new Editor::CEditor();
		m_editor->initImportProjectGUI();

		m_editorState = InitEngine;
	}
	break;
	case InitEngine:
	{
		CShaderManager::getInstance()->initBasicShader();
		CShaderManager::getInstance()->initSGDeferredShader();

		// import project
		m_editorState = Loading;
	}
	break;
	case Loading:
	{
		// loading project
		if (m_editor->updateImporting() == true)
		{
			m_editor->initEditorGUI();
			m_editorState = Running;
		}
	}
	break;
	default:
		// Running
		Editor::GUI::CGUIContext::update((float)getIrrlichtDevice()->getTimer()->getTime());
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