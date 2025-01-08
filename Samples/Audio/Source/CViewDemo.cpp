#include "pch.h"
#include "CViewDemo.h"

#include "Context/CContext.h"
#include "imgui.h"
#include "CImguiManager.h"

#include "CTestSoundComponent.h"

CViewDemo::CViewDemo() :
	m_music(NULL),
	m_sound(NULL),
	m_streamFactory(NULL)
{
	Audio::initSkylichtAudio();

	// add to support read resource on ZIP Bundle
	m_streamFactory = new CZipAudioStreamFactory();
	Audio::CAudioEngine::getSoundEngine()->registerStreamFactory(m_streamFactory);
}

CViewDemo::~CViewDemo()
{
	Audio::CAudioEngine::getSoundEngine()->destroyAllEmitter();
	Audio::CAudioEngine::getSoundEngine()->unRegisterStreamFactory(m_streamFactory);
	delete m_streamFactory;

	Audio::releaseSkylichtAudio();
}

void CViewDemo::onInit()
{
	CContext* context = CContext::getInstance();
	CCamera* camera = context->getActiveCamera();

	os::Printer::log("Init sound emitter");

	Audio::CAudioEngine* engine = Audio::CAudioEngine::getSoundEngine();

	m_music = engine->createAudioEmitter("SampleAudio/file_example_MP3_5MG.mp3", false);
	if (m_music != NULL)
	{
		m_music->setLoop(true);
		m_music->setGain(0.5f);
		m_music->play();
	}
	else
	{
		os::Printer::log("Init music emitter FAILED!");
	}

	m_sound = engine->createAudioEmitter("SampleAudio/helicopter_loop.wav", true);
	if (m_sound != NULL)
	{
		// roll off 20m
		m_sound->setRollOff(20.0f);
		m_sound->setPosition(0.0f, 0.0f, 0.0f);

		m_sound->setLoop(true);
		m_sound->play();
	}
	else
	{
		os::Printer::log("Init sound emitter FAILED!");
	}

	// atlas texture
	ITexture* texture = CTextureManager::getInstance()->getTexture("SampleAudio/helicopter.png");

	// create helicoper obj & sound
	CScene* scene = context->getScene();
	CZone* zone = scene->getZone(0);

	CGameObject* helicoper = zone->createEmptyObject();
	CTestSoundComponent* testSound = helicoper->addComponent<CTestSoundComponent>();
	testSound->setTexture(texture, 0.005f, SColor(255, 255, 255, 255));
	testSound->setBillboard(true);
	testSound->setCenter(true);
	testSound->setAudioEmitter(m_sound);
}

void CViewDemo::onDestroy()
{

}

void CViewDemo::onUpdate()
{
	CContext* context = CContext::getInstance();
	CScene* scene = context->getScene();
	if (scene != NULL)
		scene->update();

	// setup listener
	CCamera* camera = context->getActiveCamera();
	CGameObject* cameraObj = camera->getGameObject();

	core::vector3df position = cameraObj->getPosition();
	core::vector3df up = cameraObj->getUp();
	core::vector3df front = cameraObj->getFront();

	Audio::CAudioEngine* engine = Audio::CAudioEngine::getSoundEngine();
	engine->setListener(
		position.X, position.Y, position.Z,
		up.X, up.Y, up.Z,
		front.X, front.Y, front.Z
	);

	CImguiManager::getInstance()->onNewFrame();
}

void CViewDemo::onRender()
{
	CContext* context = CContext::getInstance();

	CCamera* camera = context->getActiveCamera();
	CCamera* guiCamera = context->getGUICamera();

	CScene* scene = context->getScene();

	// render scene
	if (camera != NULL && scene != NULL)
	{
		context->getRenderPipeline()->render(NULL, camera, scene->getEntityManager(), core::recti());
	}

	// render GUI
	if (guiCamera != NULL)
	{
		CGraphics2D::getInstance()->render(guiCamera);
	}

	// imgui render
	onGUI();
	CImguiManager::getInstance()->onRender();
}

void CViewDemo::onPostRender()
{

}

void CViewDemo::onGUI()
{
	bool open = true;

	ImGuiWindowFlags window_flags = 0;

	// We specify a default position/size in case there's no data in the .ini file. Typically this isn't required! We only do it to make the Demo applications a little more welcoming.
	ImGui::SetNextWindowPos(ImVec2(850, 15), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(310, 210), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Audio Test", &open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	// BEGIN WINDOW
	{
		if (ImGui::CollapsingHeader("Music", ImGuiTreeNodeFlags_DefaultOpen))
		{
			float volume = m_music->getGain();
			ImGui::SliderFloat("Music volume", &volume, 0.0f, 1.0f);
			m_music->setGain(volume);

			if (ImGui::Button(m_music->isPlaying() ? "Stop music" : "Play music"))
			{
				if (m_music->isPlaying())
					m_music->stop();
				else
					m_music->play();
			}
		}

		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Sound", ImGuiTreeNodeFlags_DefaultOpen))
		{
			float volume = m_sound->getGain();
			ImGui::SliderFloat("Sound volume", &volume, 0.0f, 1.0f);
			m_sound->setGain(volume);

			if (ImGui::Button(m_sound->isPlaying() ? "Stop sound" : "Play sound"))
			{
				if (m_sound->isPlaying())
					m_sound->stop();
				else
					m_sound->play();
			}
		}

		ImGui::End();
	}
}