#include "MInputBox.h"
#include "imgui.h"

NS_JYE_BEGIN

MInputBox::MInputBox(const String& title, const String& txt, CallFunc& func)
	: m_title(title)
	, m_txt(txt)
	, m_open(false)
	, m_callFunc(std::move(func))
{

}

MInputBox::~MInputBox()
{

}

void MInputBox::Open()
{
	if (!m_open)
	{
		m_open = true;
		ImGui::OpenPopup(m_title.c_str());
	}
}

void MInputBox::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(320, 130));
	bool b_modal = true;
	if (ImGui::BeginPopupModal(m_title.c_str(), &b_modal, ImGuiWindowFlags_NoResize))
	{
		if (m_open)
		{
			static char buf[30] = "";
			strcpy(buf, m_txt.c_str());
			bool change = ImGui::InputText("##MInputBox_input", buf, 30, ImGuiInputTextFlags_AutoSelectAll);
			ImGui::SameLine();
			m_txt = (buf);
			if (ImGui::Button("OK"))
			{
				bool valid = m_callFunc.Execute(m_txt, m_tip);
				if (valid)
				{
					Close();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				Close();
			}
			if (!m_tip.empty())
				ImGui::TextColored(ImVec4(1, 0, 0, 1), m_tip.c_str());
		}
		ImGui::EndPopup();
	}
	else if (m_open)
	{
		String temp;
		Close();
	}
}

void MInputBox::Close()
{
	m_open = false;
	//m_callFunc.Reset();
	ImGui::CloseCurrentPopup();
}

NS_JYE_END