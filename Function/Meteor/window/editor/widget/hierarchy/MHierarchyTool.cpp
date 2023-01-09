#include "MHierarchyTool.h"
#include "imgui.h"
#include "Engine/component/render/RenderComponent.h"
#include "../../system/EditorSystem.h"

NS_JYE_BEGIN

bool MHierarchyTool::NewNodeContextMenu()
{
	bool ret = ImGui::BeginPopupContextWindow("New Object", 0, true);
	if (ret)
	{
		if (ImGui::BeginMenu("3D Objects"))
		{
			static const HashMap<String, Vector<String>> menu_objects =
			{
				{ "Basic" , {"Box","Cone","Cylinder", "Sphere","Teapot","Torus","Tube", "Quad"}},
				{ "Lights" , {"AmbientLight","DirectionLight","PointLight","SpotLight"}},
				{ "Empty" , {}},
			};
			for (auto& it : menu_objects)
			{
				String name = it.first;
				if (name == "Empty")
				{
					if (ImGui::MenuItem(name.c_str()))
					{
						GObject* obj = EditorSystem::Instance()->CreateEmptyNode(name);
						EditorSystem::Instance()->Select(obj);
					}
				}
				else
				{
					Vector<String> names = it.second;
					if (ImGui::BeginMenu(name.c_str()))
					{
						for (int oi = 0; oi < names.size(); ++oi)
						{
							String& itObjectNames = names[oi];
							if (name == "Basic")
							{
								if (ImGui::MenuItem(itObjectNames.c_str()))
								{
									// TODO ´´½¨base 3d object
									//GObject* obj = EditorSystem::Instance()->CreateBasicObject(name);
									//EditorSystem::Instance()->Select(obj);
								}
							}
							else if (name == "Lights")
							{
								if (ImGui::MenuItem(itObjectNames.c_str()))
								{
									GObject* obj = EditorSystem::Instance()->CreateLight((GraphicDefine::LightType)oi);
									EditorSystem::Instance()->Select(obj);
								}
							}
							else
							{
								JYWARNING("never be call!");
							}
						}
						ImGui::EndMenu();
					}
				}
			}

			ImGui::EndMenu();
		}
	}
	return ret;
}

bool MHierarchyTool::IsNodeVisible(GObject* node)
{
	if (node
		&& !node->isLayer(MC_MASK_EDITOR_SCENE_LAYER)
		&& !node->isLayer(MC_MASK_EDITOR_UI_LAYER)
		&& !node->isLayer(MC_MASK_EDITOR_CLOTHDEBUG_LAYER))
	{
		return true;
	}
	return false;
}

Math::AABB MHierarchyTool::GetBindBox(GObject* node)
{
	Math::AABB box;

	RenderComponent* render = node->TryGetComponent<RenderComponent>();
	if (render)
	{
		box.Merge(render->GetBindBox());
	}

	auto& childs = node->GetChildren();
	for (auto& child : childs)
	{
		RenderComponent* crender = child->TryGetComponent<RenderComponent>();
		if (crender)
		{
			box.Merge(crender->GetBindBox());
		}
		box.Merge(GetBindBox(child));
	}

	return box;
}

NS_JYE_END