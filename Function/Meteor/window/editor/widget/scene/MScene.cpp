#include "MScene.h"
#include "imgui.h"
#include "../../system/MDefined.h"
#include "util/MSceneManagerExt.h"
#include "Engine/object/GObject.h"
#include "Engine/component/render/RenderComponent.h"
#include "Engine/component/camera/CameraComponent.h"
#include "Math/3DMath.h"
#include "../../system/EditorSystem.h"
#include "Engine/render/texture/TextureEntity.h"
#include "Engine/component/transform/TransformComponent.h"
#include "Core/Propery/InputPropery.h"
#include "MDrag.h"

NS_JYE_BEGIN

MScene::MScene()
	: MMemuWindowBase("Scene")
	, m_mousespeed(MDefined::MouseSpeed)
	, m_wheelspeed(MDefined::WheelSpeed)
	, m_keyspeed(MDefined::KeyBoardSpeed)
	, m_dragthreshold(MDefined::MouseDragThreshold)
	, m_keypressrate(MDefined::KeyBoardPressRate)
	, m_keypressdelay(MDefined::KeyBoardPressDelay)
	, m_isGridShow(true)
{

}

MScene::~MScene()
{

}

bool MScene::OnGui(float dt)
{
	if (IsOnGui())
	{
		int dockFlag = ImGuiWindowFlags_NoScrollbar + ImGuiWindowFlags_NoCollapse;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		bool b = ImGui::Begin(m_titleName.c_str(), &m_isOnGui, dockFlag);

		ProcessInput();

		GObject* gridNode = MSceneManagerExt::Instance()->GetGrid();
		SetGridShow(gridNode, EditorSystem::Instance()->GetShowGrid());

		ImVec2 size = ImGui::GetWindowSize();
		float h = ImGui::GetFrameHeight();
		size.y = size.y - h;

		GObject* editorCamera = MSceneManagerExt::Instance()->GetEditCamera();
		CameraComponent* cameraComponent = editorCamera->TryGetComponent<CameraComponent>();
		cameraComponent->ChangeResolution(Math::IntVec2(size.x, size.y));

		TextureEntity* editorOutput = EditorSystem::Instance()->GetEditorCameraOutput();
		const Math::IntVec2 outsize = editorOutput->GetSize();
		ImGui::Image(editorOutput, ImVec2(outsize.x, outsize.y));

		_UpdateGrid(gridNode, cameraComponent);

		MDrag::AxisDrag();

		ImGui::End();
		ImGui::PopStyleVar();
	}
	return true;
}

void MScene::SetGridShow(GObject* grid, bool show)
{
	if (m_isGridShow != show)
	{
		m_isGridShow = show;
		RenderComponent* rc = grid->TryGetComponent<RenderComponent>();
		if (show)
		{
			rc->SetRenderProperty(GraphicDefine::RP_SHOW);
		}
		else
		{
			rc->EraseRenderProperty(GraphicDefine::RP_SHOW);
		}
	}
}

void MScene::ProcessInput()
{
	GObject* editorCamera = MSceneManagerExt::Instance()->GetEditCamera();
	CameraComponent* cameraComponent = editorCamera->TryGetComponent<CameraComponent>();

	if (ImGui::IsWindowHovered())
	{
		if (ImGui::IsKeyDown(InputPropery::KB_KEY_LEFT_ALT) || ImGui::IsKeyDown(InputPropery::KB_KEY_RIGHT_ALT))
		{
			if (ImGui::IsMouseDragging(MDefined::Mouse::Left, m_dragthreshold))
			{
				ImVec2 v2 = ImGui::GetMouseDragDelta(MDefined::Mouse::Left, m_dragthreshold);
				v2.x = v2.x / 180 * Math::PI * m_mousespeed;
				v2.y = v2.y / 180 * Math::PI * m_mousespeed;
				Math::Vec3 up = cameraComponent->GetUp();
				Math::Vec3 rotatepoint = cameraComponent->GetPosition() + cameraComponent->GetForward() * 1;
				Math::Vec3 vec = cameraComponent->GetPosition() - rotatepoint;
				Math::Vec3 right = up.Cross(vec);
				Math::Quaternion rot;
				rot.FromAngleAxis(right, -v2.y);
				vec = rot * vec;
				if (up.Dot(Math::Vec3(0, 1, 0)) < 0)
					up = Math::Vec3(0, -1, 0);
				else
					up = Math::Vec3(0, 1, 0);
				
				rot.FromAngleAxis(up, -v2.x);
				vec = rot * vec;
				right = rot * right;
				up = vec.Cross(right);
				cameraComponent->LookAt(rotatepoint + vec, rotatepoint, up);
				cameraComponent->Recalculate();
				ImGui::ResetMouseDragDelta(MDefined::Mouse::Left);
			}
			return;
		}

		//Ëõ·Å
		ImGuiIO io = ImGui::GetIO();
		float vertical = io.MouseWheel;
		float horizontal = io.MouseWheelH;
		if (vertical != 0)
			cameraComponent->SetPosition(cameraComponent->GetPosition() + cameraComponent->GetForward() * vertical * m_wheelspeed);

		// Ðý×ª
		if (ImGui::IsMouseDragging(MDefined::Mouse::Right, m_dragthreshold))
		{
			ImVec2 v2 = ImGui::GetMouseDragDelta(MDefined::Mouse::Right, m_dragthreshold);
			v2.x = v2.x / 180 * Math::PI * m_mousespeed;
			v2.y = v2.y / 180 * Math::PI * m_mousespeed;
			Math::Vec3 position = cameraComponent->GetPosition();
			Math::Vec3 forward = cameraComponent->GetForward();
			Math::Vec3 up = cameraComponent->GetUp();
			Math::Vec3 right = forward.Cross(up);
			Math::Quaternion xrot;
			xrot.FromAngleAxis(right, -v2.y);
			forward = xrot * forward;
			if (up.Dot(Math::Vec3(0, 1, 0)) < 0)
				up = Math::Vec3(0, -1, 0);
			else
				up = Math::Vec3(0, 1, 0);

			xrot.FromAngleAxis(up, -v2.x);
			forward = xrot * forward;
			right = xrot * right;
			up = right.Cross(forward);
			Math::Vec3 lookatpos = position + forward;
			cameraComponent->LookAt(position, lookatpos, up);
			cameraComponent->Recalculate();
			ImGui::ResetMouseDragDelta(MDefined::Mouse::Right);
		}

		if (ImGui::IsMouseDragging(MDefined::Mouse::Mid, m_dragthreshold))
		{
			ImVec2 v2 = ImGui::GetMouseDragDelta(MDefined::Mouse::Mid, m_dragthreshold);
			v2.x *= m_mousespeed / 25;
			v2.y *= m_mousespeed / 25;
			Math::Vec3 position = cameraComponent->GetPosition();
			Math::Vec3 forward = cameraComponent->GetForward();
			Math::Vec3 up = cameraComponent->GetUp();
			Math::Vec3 right = forward.Cross(up);
			up.Normalize();
			right.Normalize();
			position = position + right * -v2.x + up * v2.y;
			cameraComponent->SetPosition(position);
			ImGui::ResetMouseDragDelta(MDefined::Mouse::Mid);
		}

		if (ImGui::IsMouseDown(MDefined::Mouse::Right))
		{
			Math::Vec3 forward = cameraComponent->GetForward();
			Math::Vec3 up = cameraComponent->GetUp();
			Math::Vec3 right = forward.Cross(up);
			forward.Normalize();
			right.Normalize();
			if (ImGui::IsKeyDown(InputPropery::KB_KEY_W))
				cameraComponent->SetPosition(cameraComponent->GetPosition()
					+ forward * ImGui::GetKeyPressedAmount(InputPropery::KB_KEY_W, m_keypressdelay, m_keypressrate) * m_keyspeed);

			if (ImGui::IsKeyDown(InputPropery::KB_KEY_S))
				cameraComponent->SetPosition(cameraComponent->GetPosition()
					- forward * ImGui::GetKeyPressedAmount(InputPropery::KB_KEY_S, m_keypressdelay, m_keypressrate) * m_keyspeed);

			if (ImGui::IsKeyDown(InputPropery::KB_KEY_A))
				cameraComponent->SetPosition(cameraComponent->GetPosition()
					- right * ImGui::GetKeyPressedAmount(InputPropery::KB_KEY_A, m_keypressdelay, m_keypressrate) * m_keyspeed);

			if (ImGui::IsKeyDown(InputPropery::KB_KEY_D))
				cameraComponent->SetPosition(cameraComponent->GetPosition()
					+ right * ImGui::GetKeyPressedAmount(InputPropery::KB_KEY_D, m_keypressdelay, m_keypressrate) * m_keyspeed);

			if (ImGui::IsKeyDown(InputPropery::KB_KEY_Q))
				cameraComponent->SetPosition(cameraComponent->GetPosition()
					- up * ImGui::GetKeyPressedAmount(InputPropery::KB_KEY_Q, m_keypressdelay, m_keypressrate) * m_keyspeed);

			if (ImGui::IsKeyDown(InputPropery::KB_KEY_E))
				cameraComponent->SetPosition(cameraComponent->GetPosition()
					+ up * ImGui::GetKeyPressedAmount(InputPropery::KB_KEY_E, m_keypressdelay, m_keypressrate) * m_keyspeed);
		}
	}
	
}

void MScene::_UpdateGrid(GObject* grid, CameraComponent* cam)
{
	TransformComponent* transformComponent = grid->TryGetComponent<TransformComponent>();
	Math::Vec3 possub = cam->GetPosition();

	float offsetx = possub.x > 0 ? floor(possub.x) : -floor(-possub.x);
	float offsetz = possub.z > 0 ? floor(possub.z) : -floor(-possub.z);
	if (!Math::Equals(offsetx, 0.0f) || !Math::Equals(offsetz, 0.0f))
	{
		transformComponent->SetLocalPosition(Math::Vec3(offsetx, 0.0, offsetz));
	}
}

NS_JYE_END