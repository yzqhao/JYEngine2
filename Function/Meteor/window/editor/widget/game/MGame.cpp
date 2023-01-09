#include "MGame.h"
#include "imgui.h"
#include "util/MSceneManagerExt.h"
#include "Engine/render/RenderTargetEntity.h"
#include "Engine/object/Scene.h"

NS_JYE_BEGIN

struct GameSizeInfo
{
	String m_strKey;
	float m_radio;
};

static const Vector<GameSizeInfo> s_ViewSize =
{
	{"1 : 2", 1.0 / 2},
	{"2 : 1", 2.0 / 1},
	{"3 : 4", 3.0 / 4},
	{"4 : 3", 4.0 / 3},
	{"16 : 9", 16.0 / 9},
	{"9 : 16", 9.0 / 16},
};

MGame::MGame()
	: MMemuWindowBase("Game")
	, m_ratio(16.0/9)
{

}

MGame::~MGame()
{

}

// 限制view分辨率是偶数(因为yuv需要降采样)
static void CustomConstraintsEven(ImGuiSizeCallbackData* data)
{
	data->DesiredSize = ImVec2(((int)data->DesiredSize.x) / 2 * 2, ((int)data->DesiredSize.y) / 2 * 2);
}

bool MGame::OnGui(float dt)
{
	if (IsOnGui())
	{
		int dockFlag = ImGuiWindowFlags_NoScrollbar + ImGuiWindowFlags_NoCollapse;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX), CustomConstraintsEven);
		bool b = ImGui::Begin(m_titleName.c_str(), &m_isOnGui, dockFlag);

		ImVec2 size = ImGui::GetWindowSize();
		float h = ImGui::GetFrameHeight();
		size.y = size.y - h;

		Menu();

		if (size.x / size.y > m_ratio)
			m_windowSize = Math::IntVec2(floor(size.y * m_ratio), size.y);
		else
			m_windowSize = Math::IntVec2(size.x, floor(size.x * (1 / m_ratio)));

		m_windowSize.x = m_windowSize.x - fmod(m_windowSize.x, 2);
		m_windowSize.y = m_windowSize.y - fmod(m_windowSize.y, 2);

		Scene* gameScene = MSceneManagerExt::Instance()->GetPreviewScene();
		Scene* editScene = MSceneManagerExt::Instance()->GetEditScene();
		Scene* utilityScene = SceneManager::Instance()->GetUtilityScene();
		RenderTargetEntity* renderTarget = gameScene->GetDefaultRenderTarget();
		TextureEntity* renderOutput = renderTarget->GetAttachment(RHIDefine::TA_COLOR_0);

		gameScene->ChangeDefaultResolution(m_windowSize);
		editScene->ChangeDefaultResolution(m_windowSize);
		utilityScene->ChangeDefaultResolution(m_windowSize);

		ImVec2 cursorPos(0, h);
		cursorPos.x = (size.x - m_windowSize.x) * 0.5;
		cursorPos.y = (h + (size.y - m_windowSize.y) * 0.5);
		ImGui::SetCursorPos(cursorPos);

		const Math::IntVec2& imagesize = renderOutput->GetSize();
		ImGui::Image(renderOutput, ImVec2(imagesize.x, imagesize.y));

		ImGui::End();
		ImGui::PopStyleVar();
	}
	return true;
}

void MGame::Menu()
{

}

NS_JYE_END