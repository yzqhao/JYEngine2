#include "MDrag.h"
#include "Engine/object/GObject.h"
#include "Engine/component/transform/TransformComponent.h"
#include "Engine/component/camera/CameraComponent.h"
#include "Core/Propery/InputPropery.h"
#include "../../system/EditorSystem.h"
#include "util/MSceneManagerExt.h"
#include "../../command/commands/MCmdPropertyChange.h"
#include "../../command/MCommandManager.h"

#include "imgui.h"
#include "imguizmo.h"
#include "Engine/component/GizmoConfig.h"

NS_JYE_BEGIN

static TransformComponent* g_trans = nullptr;
static GObject* g_object = nullptr;
static Math::Vec3 g_scaleresult, g_transresult;
static Math::Quaternion g_rotateresult;

ImGui::operation g_operation = ImGui::op_null;

static void ImGui_Manipulate(Math::Vec3& scale, Math::Vec3& trans, Math::Quaternion& rot, const Math::Mat4& view, const Math::Mat4& projection, ImGui::operation op, ImGui::mode mod, const Math::Mat4& matrix, bool show2D)
{
	ImGui::manipulate(&view.a11, &projection.a11, op, mod, (float*)&matrix.a11, nullptr, nullptr, nullptr, nullptr, show2D);
	matrix.Decompose(&scale, &rot, &trans);
}

namespace MDrag
{
	void PickNode()
	{
		GObject* editorCamera = MSceneManagerExt::Instance()->GetEditCamera();
		CameraComponent* cameraComponent = editorCamera->TryGetComponent<CameraComponent>();
		if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered()	//只有鼠标点击scene窗口里面的区域才会刷新nodelist和nodepicked
			&& ImGui::IsWindowHovered()) // 点击场景切换按钮不触发拾取
		{
			TextureEntity* out = EditorSystem::Instance()->GetEditorCameraOutput();
			Math::IntVec2 imgsize = out->GetSize();
			ImVec2 regmin = ImGui::GetWindowContentRegionMin();
			ImVec2 winpos = ImGui::GetWindowPos();
			ImVec2 mousepos = ImGui::GetMousePos();
			ImVec2 pos = ImVec2(mousepos.x - regmin.x - winpos.x, mousepos.y - regmin.y - winpos.y);
			// 鼠标相对渲染屏幕的坐标
			// 注意最好灰色屏幕紧贴着工具栏 不要占用原来的渲染纹理增加按钮  否则坐标值会不正确
			Math::Vec2 coordOfOpengl = Math::Vec2(pos.x / imgsize.x * 2.0 - 1.0, (imgsize.y - pos.y) / imgsize.y * 2.0 - 1.0);	//转化成opengl的坐标系 - 1, 1之间
			Vector<GObject*> listnode = cameraComponent->PickNodes(coordOfOpengl);
			GObject* nodepicked = listnode.empty() ? nullptr : listnode[0];

			if (nodepicked)
			{
				if (nodepicked->GetName() == GizmoConfig::GIZMO_CAMERA || nodepicked->GetName() == GizmoConfig::GIZMO_LIGHT) //拾取到贴图则返回根节点
					nodepicked = nodepicked->GetRoot();
			}
			EditorSystem::Instance()->Select(nodepicked);
		}
	}

	void AxisDrag()
	{
		ImGui::mode mode = ImGui::local;
		ImGui::operation operation = ImGui::translate;
		if (!ImGui::IsMouseDown(1) && !ImGui::IsAnyItemActive()) // 右键没有按下  物体没有被拖拽ing
		{
			if (ImGui::IsKeyDown(InputPropery::KB_KEY_W))
			{
				g_operation = (ImGui::translate);
				operation = ImGui::translate;
			}
			if (ImGui::IsKeyDown(InputPropery::KB_KEY_E))
			{
				g_operation = (ImGui::rotate);
				operation = ImGui::rotate;
			}
			if (ImGui::IsKeyDown(InputPropery::KB_KEY_R))
			{
				g_operation = (ImGui::scale);
				operation = ImGui::scale;
			}
		}

		g_object = EditorSystem::Instance()->GetSelectedGObject();
		Math::Vec3 pre_scaleresult = g_scaleresult;
		Math::Vec3 pre_transresult = g_transresult;
		Math::Quaternion pre_rotateresult = g_rotateresult;
		if (g_object && g_object->TryGetComponent<TransformComponent>())
		{
			ImVec2 p = ImGui::GetItemRectMin();
			ImVec2 s = ImGui::GetItemRectSize();
			ImGui::set_view_rect(p.x, p.y, s.x, s.y);
			g_trans = g_object->TryGetComponent<TransformComponent>();
			if (g_operation == ImGui::translate)
				operation = ImGui::translate;
			else if (g_operation == ImGui::rotate)
				operation = ImGui::rotate;
			else if(g_operation == ImGui::scale)
				operation = ImGui::scale;

			Math::Mat4 transmatrix = g_trans->GetWorldTransform();
			GObject* editorCamera = MSceneManagerExt::Instance()->GetEditCamera();
			CameraComponent* cameraComponent = editorCamera->TryGetComponent<CameraComponent>();
			Math::Mat4 cameraview = cameraComponent->GetView();
			Math::Mat4 cameraproj = cameraComponent->GetProject();
			ImGui_Manipulate(g_scaleresult, g_transresult, g_rotateresult, cameraview, cameraproj, operation, mode, transmatrix, false);
		}

		if (ImGui::is_using()) // 坐标轴被选中了就进行几何变换
		{
			uint64 StaticID = g_trans->GetStaticID();
			if (pre_scaleresult != g_scaleresult)
			{
				M_CMD_EXEC_BEGIN;
				MCommandManager::Instance()->Execute<MCmdPropertyChangeVec3>(StaticID, "m_LocalScale", g_scaleresult);
				M_CMD_EXEC_END;
			}
			if (pre_rotateresult != g_rotateresult)
			{
				M_CMD_EXEC_BEGIN;
				MCommandManager::Instance()->Execute<MCmdPropertyChangeQuaternion>(StaticID, "m_LocalRotation", g_rotateresult);
				M_CMD_EXEC_END;
			}
			if (pre_transresult != g_transresult)
			{
				M_CMD_EXEC_BEGIN;
				MCommandManager::Instance()->Execute<MCmdPropertyChangeVec3>(StaticID, "m_LocalPosition", g_transresult);
				M_CMD_EXEC_END;
			}
		}
		else
		{
			PickNode();
		}
	}

	uint64 GetTheObjectID()
	{
		return g_object ? g_object->GetStaticID() : 0;
	}

	void DropObject()
	{
		g_object = nullptr;
	}
};

NS_JYE_END