#include "MImGuiUtil.h"
#include "imgui.h"
#include "imgui_internal.h"

NS_JYE_BEGIN

void MImGuiUtil::CircleLoadingBar()
{
	const ImU32 color = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
	const float radius = 50;
	const int thickness = 10;
	const char* label = "##spinner";
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

	const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
	ImGui::ItemSize(bb, style.FramePadding.y);

	// Render
	window->DrawList->PathClear();

	int num_segments = 120;
	int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

	const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
	const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

	const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

	for (int i = 0; i < num_segments; i++) {
		const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
		window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
			centre.y + ImSin(a + g.Time * 8) * radius));
	}

	window->DrawList->PathStroke(color, false, thickness);
}

NS_JYE_END