#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleSocketIO.h"

#include "GridPlane/CGridPlane.h"
#include "imgui.h"
#include "CImguiManager.h"

#include "json/json.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleSocketIO* app = new SampleSocketIO();
	getApplication()->registerAppEvent("SampleSocketIO", app);
}

SampleSocketIO::SampleSocketIO() :
	m_scene(NULL),
	m_forwardRP(NULL),
	m_largeFont(NULL)
{
	CImguiManager::createGetInstance();

	m_scrollToBottom = true;
	m_autoScroll = true;
	m_inputBuffer[0] = 0;
}

SampleSocketIO::~SampleSocketIO()
{
	delete m_scene;
	delete m_largeFont;
	delete m_forwardRP;

	delete m_io;

	CImguiManager::releaseInstance();
}

void SampleSocketIO::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

	app->showDebugConsole();

	// load "BuiltIn.zip" to read files inside it
	app->getFileSystem()->addFileArchive(app->getBuiltInPath("BuiltIn.zip"), false, false);

	// init segoeuil.ttf inside BuiltIn.zip
	CGlyphFreetype* freetypeFont = CGlyphFreetype::getInstance();
	freetypeFont->initFont("Segoe UI Light", "BuiltIn/Fonts/segoeui/segoeuil.ttf");

	// load basic shader
	CShaderManager* shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();

	// create a Scene
	m_scene = new CScene();

	// create a Zone in Scene
	CZone* zone = m_scene->createZone();

	// create 2D camera
	CGameObject* guiCameraObject = zone->createEmptyObject();
	m_guiCamera = guiCameraObject->addComponent<CCamera>();
	m_guiCamera->setProjectionType(CCamera::OrthoUI);

	// create 3D camera
	CGameObject* camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);

	m_camera = camObj->getComponent<CCamera>();
	m_camera->setPosition(core::vector3df(0.0f, 1.5f, 4.0f));
	m_camera->lookAt(core::vector3df(0.0f, 0.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// 3d grid
	CGameObject* grid = zone->createEmptyObject();
	grid->addComponent<CGridPlane>();

	// lighting
	CGameObject* lightObj = zone->createEmptyObject();
	CDirectionalLight* directionalLight = lightObj->addComponent<CDirectionalLight>();
	SColor c(255, 255, 244, 214);
	directionalLight->setColor(SColorf(c));

	CTransformEuler* lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(0.0f, -1.5f, 2.0f);
	lightTransform->setOrientation(direction, Transform::Oy);

	m_largeFont = new CGlyphFont();
	m_largeFont->setFont("Segoe UI Light", 50);

	// create 2D Canvas
	CGameObject* canvasObject = zone->createEmptyObject();
	CCanvas* canvas = canvasObject->addComponent<CCanvas>();

	// create UI Text in Canvas
	CGUIText* textLarge = canvas->createText(m_largeFont);
	textLarge->setText("SampleSocketIO");
	textLarge->setTextAlign(EGUIHorizontalAlign::Left, EGUIVerticalAlign::Top);

	// rendering pipe line
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	m_forwardRP = new CForwardRP();
	m_forwardRP->initRender(w, h);

	// see Readme.md on Samples\SocketIOServer
	// that how to start the server
	m_io = new CSocketIO("localhost:8080");

	m_io->OnConnected = [&]()
	{
		m_logs.push_back("[success] Connected");
	};

	m_io->OnConnectFailed = [&]()
	{
		m_logs.push_back("[error] Could not open connection to the localhost:8080");
	};

	m_io->init();
}

void SampleSocketIO::onUpdate()
{
	// update application
	m_scene->update();

	m_io->update();

	if (m_io->isConnected())
	{
		// on receive message
		// see Samples/SocketIOServer/index.js
		m_io->OnMessage = [&](const std::string& msg)
		{
			Json::Reader reader;
			Json::Value obj;

			reader.parse(msg, obj);

			const Json::Value& message = obj[(Json::UInt)0];
			const Json::Value& data = obj[(Json::UInt)1];

			if (message.asString() == "message")
			{
				std::string user = data["user"].asCString();
				std::string chatMsg = data["message"].asCString();

				std::string log;

				if (user == m_io->getSocketID())
					log = user + "[me]:" + chatMsg;
				else
					log = user + "# :" + chatMsg;

				m_logs.push_back(log);
			}
			else if (message.asString() == "join")
			{
				std::string user = data["user"].asCString();
				std::string log;
				log = user + "join the chat room";
				m_logs.push_back(log);
			}
			else if (message.asString() == "left")
			{
				std::string user = data["user"].asCString();
				std::string log;
				log = user + "has left the chat room";
				m_logs.push_back(log);
			}
		};

		m_io->OnMessageAsk = [&](const std::string& msg, int id)
		{
			m_logs.push_back(msg);
		};
	}

	CImguiManager::getInstance()->onNewFrame();

	// on gui
	{
		bool open = true;

		ImGuiWindowFlags window_flags = 0;

		// We specify a default position/size in case there's no data in the .ini file. Typically this isn't required! We only do it to make the Demo applications a little more welcoming.
		ImGui::SetNextWindowPos(ImVec2(130, 50), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(870, 550), ImGuiCond_FirstUseEver);

		if (!ImGui::Begin("Socket.io Chat", &open, window_flags))
		{
			// Early out if the window is collapsed, as an optimization.
			ImGui::End();
			return;
		}

		const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
		for (int i = 0; i < (int)m_logs.size(); i++)
		{
			const char* item = m_logs[i].c_str();

			// Normally you would store more information in your item (e.g. make Items[] an array of structure, store color/type etc.)
			bool pop_color = false;
			if (strstr(item, "[error]"))
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
				pop_color = true;
			}
			else if (strstr(item, "[success]"))
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
				pop_color = true;
			}
			else if (strstr(item, "[me]"))
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.4f, 1.0f));
				pop_color = true;
			}
			else if (strstr(item, "# "))
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f));
				pop_color = true;
			}

			ImGui::TextUnformatted(item);
			if (pop_color)
				ImGui::PopStyleColor();
		}

		if (m_scrollToBottom || (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			ImGui::SetScrollHereY(1.0f);
		m_scrollToBottom = false;

		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::Separator();

		// Chat command-line
		bool reclaimFocus = false;
		if (ImGui::InputText("Input", m_inputBuffer, IM_ARRAYSIZE(m_inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			m_io->emit("chat", "message", m_io->toStringParam(m_inputBuffer), false);
			m_inputBuffer[0] = 0;
			reclaimFocus = true;
		}

		// Auto-focus on window apparition
		ImGui::SetItemDefaultFocus();
		if (reclaimFocus)
			ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

		ImGui::End();

		// DEMO HOW TO USE IMGUI (SEE THIS FUNCTION)
		// ImGui::ShowDemoWindow(&open);
	}
}

void SampleSocketIO::onRender()
{
	// render 3d scene
	m_forwardRP->render(NULL, m_camera, m_scene->getEntityManager(), core::recti());

	// render text in gui camera
	CGraphics2D::getInstance()->render(m_guiCamera);

	// render imgui
	CImguiManager::getInstance()->onRender();
}

void SampleSocketIO::onPostRender()
{
	// post render application
}

bool SampleSocketIO::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleSocketIO::onResize(int w, int h)
{
	// on window size changed
	if (m_forwardRP != NULL)
		m_forwardRP->resize(w, h);
}

void SampleSocketIO::onResume()
{
	// resume application
}

void SampleSocketIO::onPause()
{
	// pause application
}

void SampleSocketIO::onQuitApp()
{
	// end application
	delete this;
}