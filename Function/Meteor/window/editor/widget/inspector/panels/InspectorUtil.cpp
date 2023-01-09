#include "InspectorUtil.h"
#include "imgui.h"
#include "Engine/render/texture/TextureEntity.h"
#include "../../../system/MDefined.h"
#include "Engine/Engine.h"

NS_JYE_BEGIN

bool InspectorUtil::Vector1(float& vec, const String& name, float speed, const String& hashname, float min, float max, const String& hintname)
{
	ImGui::Text(name.c_str());
	if (!hintname.empty())
		InspectorUtil::Hint(hintname);
	ImGui::SameLine();
	String hash = !hashname.empty() ? hashname : "vector1";
	bool noInput = (min == 0.0 && max == 0.0) ? true : false;
	bool ischange = ImGui::DragFloat(("##"+name+hash).c_str(), &vec, speed, min, max);
	if (!noInput)
	{
		if (vec < min) vec = min;
		else if (vec > max) vec = max;
	}
	return ischange;
}

bool InspectorUtil::Vector1_dec(float& vec, const String& name, float speed, const String& hashname, float min, float max)
{
	ImGui::Text(name.c_str());
	ImGui::SameLine();
	String hash = !hashname.empty() ? hashname : "vector1";
	bool noInput = (min == 0.0 && max == 0.0) ? true : false;
	bool ischange = ImGui::DragFloat(("##" + name + hash).c_str(), &vec, speed, min, max, "%.4f");
	if (!noInput)
	{
		if (vec < min) vec = min;
		else if (vec > max) vec = max;
	}
	return ischange;
}

bool InspectorUtil::IVector1(int& vec, const String& name, float speed, const String& hashname, int min, int max)
{
	ImGui::Text(name.c_str());
	ImGui::SameLine();
	String hash = !hashname.empty() ? hashname : "vector1";
	bool noInput = (min == 0 && max == 0) ? true : false;
	bool ischange = ImGui::DragInt(("##" + name + hash).c_str(), &vec, speed, min, max);
	if (!noInput)
	{
		if (vec < min) vec = min;
		else if (vec > max) vec = max;
	}
	return ischange;
}

bool InspectorUtil::Vector2(Math::Vec2& vec, const String& name, float speed, const String& hashname, float min, float max, const String& hintname)
{
	ImGui::Text(name.c_str());
	if (!hintname.empty())
		InspectorUtil::Hint(hintname);
	ImGui::SameLine();
	String hash = !hashname.empty() ? hashname : std::to_string((size_t)&vec);
	bool ischange = ImGui::DragFloat2(("##" + name + hash).c_str(), vec.GetPtr(), speed, min, max);
	return ischange;
}

bool InspectorUtil::IVector2(Math::IntVec2& vec, const String& name, float speed, const String& hashname, int min, int max)
{
	ImGui::Text(name.c_str());
	ImGui::SameLine();
	String hash = !hashname.empty() ? hashname : std::to_string((size_t)&vec);
	bool ischange = ImGui::DragInt2(("##" + name + hash).c_str(), vec.GetPtr(), speed, min, max);
	return ischange;
}

bool InspectorUtil::Vector3(Math::Vec3& vec, const String& name, float speed, const String& hashname, float min, float max, const String& hintname)
{
	ImGui::Text(name.c_str());
	if (!hintname.empty())
		InspectorUtil::Hint(hintname);
	ImGui::SameLine();
	String hash = !hashname.empty() ? hashname : std::to_string((size_t)&vec);
	bool ischange = ImGui::DragFloat3(("##" + name + hash).c_str(), vec.GetPtr(), speed, min, max);
	return ischange;
}

bool InspectorUtil::Vector3_dec(Math::Vec3& vec, const String& name, float speed, const String& hashname, float min, float max)
{
	ImGui::Text(name.c_str());
	ImGui::SameLine();
	String hash = !hashname.empty() ? hashname : std::to_string((size_t)&vec);
	bool ischange = ImGui::DragFloat3(("##" + name + hash).c_str(), vec.GetPtr(), speed, min, max, "%.4f");
	return ischange;
}

bool InspectorUtil::EularAngleInspect(Math::Vec3& vec, const String& name, float speed, const String& hashname)
{
	ImGui::Text(name.c_str());
	ImGui::SameLine();
	String hash = !hashname.empty() ? hashname : std::to_string((size_t)&vec);
	Math::Vec3 v = vec / Math::PI * 180;
	bool ischange = ImGui::DragFloat3(("##" + name + hash).c_str(), v.GetPtr(), speed);
	vec = v * Math::PI / 180;
	return ischange;
}

bool InspectorUtil::Vector4(Math::Vec4& vec, const String& name, float speed, const String& hashname, const String& hintname)
{
	ImGui::Text(name.c_str());
	if (!hintname.empty())
		InspectorUtil::Hint(hintname);
	ImGui::SameLine();
	String hash = !hashname.empty() ? hashname : std::to_string((size_t)&vec);
	bool ischange = ImGui::DragFloat4(("##" + name + hash).c_str(), (float*)vec.GetPtr());
	return ischange;
}

bool InspectorUtil::Vector4_dec(Math::Vec4& vec, const String& name, float speed, const String& hashname, float min, float max, const String& hintname)
{
	ImGui::Text(name.c_str());
	if (!hintname.empty())
		InspectorUtil::Hint(hintname);
	ImGui::SameLine();
	String hash = !hashname.empty() ? hashname : std::to_string((size_t)&vec);
	bool ischange = ImGui::DragFloat4(("##" + name + hash).c_str(), (float*)vec.GetPtr(), speed, min, max, "%.3f", 1.0);
	return ischange;
}

bool InspectorUtil::Quaternion(Math::Quaternion& vec, const String& name, float speed, const String& hashname)
{
	ImGui::Text(name.c_str());
	ImGui::SameLine();
	String hash = !hashname.empty() ? hashname : std::to_string((size_t)&vec);
	bool ischange = ImGui::DragFloat4(("##" + name + hash).c_str(), (float*)vec.GetPtr(), speed);
	return ischange;
}

bool InspectorUtil::FileSelecter(const String& name, const String& path, const String& btnName, Object* filter, const String& hash,
	MSelectWindow::SelectDelegate& callback, const String& uniformname, RHIDefine::TextureType textype, const Vector<String>& extraPath)
{
	Rtti* type_rtti = nullptr;
	Vector<String> filterNames;
	if (filter->IsSameType(TextureEntity::RTTI()))
	{
		type_rtti = &TextureEntity::RTTI();
		if (textype == RHIDefine::TEXTURE_CUBE_MAP)
			filterNames = { "hdr" };
		else
		{
			filterNames = MDefined::FileTypeList[MDefined::Texture];
			filterNames.insert(filterNames.end(), 
				MDefined::FileTypeList[MDefined::FrameAnimation].begin(), MDefined::FileTypeList[MDefined::FrameAnimation].end());
		}
	}
	else
	{
		JY_ASSERT(false);
	}
	
	Math::IntVec4 size =	Engine::Instance()->GetMainViewSize();
	ImVec2 btnSize = ImVec2(size.z / 8.0, size.w / 45.0);
	ImVec2 btnSize2 = ImVec2(size.z / 8.0, size.w / 45.0);

	if (!uniformname.empty())
	{
		ImGui::Text(uniformname.c_str()); // 反射面板调用 看到uniform字符串
		Hint(name); // tips是说明
	}
	else
	{ 
		ImGui::Text(name.c_str());  //其他地方调用
	}
	
	ImGui::SameLine();
	ImGuiStyle style = ImGui::GetStyle();
	ImVec4 gray = style.Colors[ImGuiCol_FrameBg];
	ImGui::PushStyleColor(ImGuiCol_Button, gray);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, gray);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, gray);
	bool istouch = ImGui::Button((path+"##"+name).c_str(), btnSize2);
	ImGui::PopStyleColor(3);
	ImGui::SameLine();

	if (ImGui::Button((btnName+"##"+ hash).c_str(), btnSize))
	{
		g_SeclectWindow->Open(filterNames, callback, true, extraPath);
		g_SeclectWindow->SetRttiFilter(&TextureEntity::RTTI());
	}

	return istouch;
}

void InspectorUtil::ErrorView(const String& name, const String& text, const String& moreHint, ErrorDelegate& deleteEvent)
{
	ImGui::OpenPopup(name.c_str());
	bool bOpen = true;
	if (ImGui::BeginPopupModal(name.c_str(), &bOpen, ImGuiWindowFlags_AlwaysAutoResize + ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::Text(text.c_str());
		ImGui::Separator();
		ImGui::Text(moreHint.c_str());
		ImGui::Dummy(ImVec2(100, 0));
		ImGui::SameLine();
		if (ImGui::Button("ok", ImVec2(120, 0)))
		{
			if (deleteEvent.IsBound())
			{
				deleteEvent.Execute();
			}
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void InspectorUtil::Hint(const String& str)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 15.0);
		ImGui::TextUnformatted(str.c_str());
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

NS_JYE_END