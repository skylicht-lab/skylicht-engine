#include "pch.h"
#include "SkylichtEngine.h"
#include "SampleHttpRequest.h"

#include "GridPlane/CGridPlane.h"

#include "imgui.h"
#include "CImguiManager.h"

void installApplication(const std::vector<std::string>& argv)
{
	SampleHttpRequest* app = new SampleHttpRequest();
	getApplication()->registerAppEvent("SampleHttpRequest", app);
}

SampleHttpRequest::SampleHttpRequest() :
	m_scene(NULL),
	m_forwardRP(NULL),
	m_largeFont(NULL)
{
	CImguiManager::createGetInstance();

	m_httpRequest = Network::IHttpRequest::create(new Network::CHttpStream(1024 * 1024));

	m_scrollToBottom = true;
	m_autoScroll = true;
	strcpy(m_inputBuffer, "https://www.google.com");
}

SampleHttpRequest::~SampleHttpRequest()
{
	delete m_scene;
	delete m_largeFont;
	delete m_forwardRP;

	delete m_httpRequest;

	CImguiManager::releaseInstance();
}

void SampleHttpRequest::onInitApp()
{
	// init application
	CBaseApp* app = getApplication();

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
	textLarge->setText("SampleHttpRequest");
	textLarge->setTextAlign(EGUIHorizontalAlign::Left, EGUIVerticalAlign::Top);

	// rendering pipe line
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	m_forwardRP = new CForwardRP();
	m_forwardRP->initRender(w, h);
}

void SampleHttpRequest::onUpdate()
{
	// update application
	m_scene->update();

	// update request
	if (m_httpRequest->updateRequest() == true)
	{
		Network::IHttpStream* stream = m_httpRequest->getStream();

		int requestId = m_httpRequest->getRequestID();
		if (requestId > 0)
		{
			int code = m_httpRequest->getResponseCode();
			if (code <= 0)
			{
				m_logs.push_back(std::string("[error] ") + std::to_string(code));
			}
			else
			{
				m_logs.push_back(std::string("[success] ") + std::to_string(code));

				const unsigned char* data = stream->getData();
				int size = stream->getDataSize();
				if (size > 0)
				{
					std::string line;
					for (int i = 0; i < size; i++)
					{
						if (data[i] == '\r' || data[i] == '\n')
						{
							m_logs.push_back(line);
							line.clear();

							if (m_logs.size() > 200)
								m_logs.erase(m_logs.begin());
						}
						else
						{
							line += data[i];
						}
					}
				}
			}

			// clear flag
			m_httpRequest->setRequestID(-1);
		}
	}

	// imgui gui
	CImguiManager::getInstance()->onNewFrame();
	onImGui();
}

void SampleHttpRequest::onImGui()
{
	bool open = true;

	ImGuiWindowFlags window_flags = 0;

	// We specify a default position/size in case there's no data in the .ini file. Typically this isn't required! We only do it to make the Demo applications a little more welcoming.
	ImGui::SetNextWindowPos(ImVec2(130, 50), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(870, 550), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Http request test", &open, window_flags))
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
	if (ImGui::InputText("Url", m_inputBuffer, IM_ARRAYSIZE(m_inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
	{
		m_httpRequest->setRequestID(1);
		m_httpRequest->setURL(m_inputBuffer);
		m_httpRequest->sendRequest();
		reclaimFocus = true;
	}

	ImGui::SameLine();

	if (ImGui::Button("Send request"))
	{
		m_httpRequest->setRequestID(1);
		m_httpRequest->setURL(m_inputBuffer);
		m_httpRequest->sendRequest();
	}

	ImGui::SameLine();

	if (ImGui::Button("Clear"))
	{
		m_logs.clear();
	}

	// Auto-focus on window apparition
	ImGui::SetItemDefaultFocus();
	if (reclaimFocus)
		ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

	ImGui::End();
}

void SampleHttpRequest::onRender()
{
	// render 3d scene
	m_forwardRP->render(NULL, m_camera, m_scene->getEntityManager(), core::recti());

	// render text in gui camera
	CGraphics2D::getInstance()->render(m_guiCamera);

	// render imgui
	CImguiManager::getInstance()->onRender();
}

void SampleHttpRequest::onPostRender()
{
	// post render application
}

bool SampleHttpRequest::onBack()
{
	// on back key press
	// return TRUE will run default by OS (Mobile)
	// return FALSE will cancel BACK FUNCTION by OS (Mobile)
	return true;
}

void SampleHttpRequest::onResize(int w, int h)
{
	// on window size changed
	if (m_forwardRP != NULL)
		m_forwardRP->resize(w, h);
}

void SampleHttpRequest::onResume()
{
	// resume application
}

void SampleHttpRequest::onPause()
{
	// pause application
}

void SampleHttpRequest::onQuitApp()
{
	// end application
	delete this;
}