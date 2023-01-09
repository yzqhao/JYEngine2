#include "imgui.h"
#include "imgui_internal.h"
#include <math.h>
#include <vector>
#include <string>
#include <map>
#include <limits>
#include <iostream>

static const float NODE_SLOT_RADIUS = 4.0f;

static const float GRADIENT_BAR_WIDGET_HEIGHT = 25;
static const float GRADIENT_BAR_EDITOR_HEIGHT = 40;
static const float GRADIENT_MARK_DELETE_DIFFY = 40;

static bool CurMouseRightDown = false;
static bool Float2UIActive = false;
static bool NextFrameOpenFloat2UI = false;
static int  RightDownCurIndex = -1;
static int  RightDownPointIndex = -1;
//

ImGradient::ImGradient()
{
	addMark(0.0f, ImColor(1.0f, 1.0f, 1.0f));
	addMark(1.0f, ImColor(1.0f, 1.0f, 1.0f));
}

ImGradient::~ImGradient()
{
	for (ImGradientMark* mark : m_marks)
	{
		delete mark;
	}
}

void ImGradient::addMark(float position, ImColor const color)
{
	position = ImClamp(position, 0.0f, 1.0f);
	ImGradientMark* newMark = new ImGradientMark();
	newMark->position = position;
	newMark->color[0] = color.Value.x;
	newMark->color[1] = color.Value.y;
	newMark->color[2] = color.Value.z;

	m_marks.push_back(newMark);

	refreshCache();
}

void ImGradient::removeMark(ImGradientMark* mark)
{
	m_marks.remove(mark);
	refreshCache();
}

void ImGradient::removeAllMark()
{
	std::list<ImGradientMark*>::iterator iter;
	for (iter = m_marks.begin(); iter != m_marks.end();)
	{
		iter = m_marks.erase(iter);
	}
	refreshCache();
}

void ImGradient::getColorAt(float position, float* color) const
{
	position = ImClamp(position, 0.0f, 1.0f);
	int cachePos = (position * 255);
	cachePos *= 3;
	color[0] = m_cachedValues[cachePos + 0];
	color[1] = m_cachedValues[cachePos + 1];
	color[2] = m_cachedValues[cachePos + 2];
}

void ImGradient::computeColorAt(float position, float* color) const
{
	position = ImClamp(position, 0.0f, 1.0f);

	ImGradientMark* lower = nullptr;
	ImGradientMark* upper = nullptr;

	for (ImGradientMark* mark : m_marks)
	{
		if (mark->position < position)
		{
			if (!lower || lower->position < mark->position)
			{
				lower = mark;
			}
		}

		if (mark->position >= position)
		{
			if (!upper || upper->position > mark->position)
			{
				upper = mark;
			}
		}
	}

	if (upper && !lower)
	{
		lower = upper;
	}
	else if (!upper && lower)
	{
		upper = lower;
	}
	else if (!lower && !upper)
	{
		color[0] = color[1] = color[2] = 0;
		return;
	}

	if (upper == lower)
	{
		color[0] = upper->color[0];
		color[1] = upper->color[1];
		color[2] = upper->color[2];
	}
	else
	{
		float distance = upper->position - lower->position;
		float delta = (position - lower->position) / distance;

		//lerp
		color[0] = ((1.0f - delta) * lower->color[0]) + ((delta)* upper->color[0]);
		color[1] = ((1.0f - delta) * lower->color[1]) + ((delta)* upper->color[1]);
		color[2] = ((1.0f - delta) * lower->color[2]) + ((delta)* upper->color[2]);
	}
}

void ImGradient::refreshCache()
{
	m_marks.sort([](const ImGradientMark * a, const ImGradientMark * b) { return a->position < b->position; });

	for (int i = 0; i < 256; ++i)
	{
		computeColorAt(i / 255.0f, &m_cachedValues[i * 3]);
	}
}


namespace ImGui
{

	bool CheckboxEx(const char* label, bool* v)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		ImVec2 dummy_size = ImVec2(CalcItemWidth() - label_size.y - style.FramePadding.y * 2 - style.ItemSpacing.x, label_size.y);
		Dummy(dummy_size);
		SameLine();
		const ImRect check_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(label_size.y + style.FramePadding.y * 2, label_size.y + style.FramePadding.y * 2)); // We want a square shape to we use Y twice
		ItemSize(check_bb, style.FramePadding.y);

		ImRect total_bb = check_bb;
		if (label_size.x > 0)
			SameLine(0, style.ItemInnerSpacing.x);
		const ImRect text_bb(window->DC.CursorPos + ImVec2(0, style.FramePadding.y), window->DC.CursorPos + ImVec2(0, style.FramePadding.y) + label_size);
		if (label_size.x > 0)
		{
			ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
			total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
		}

		if (!ItemAdd(total_bb, id))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
		if (pressed)
			*v = !(*v);

		RenderFrame(check_bb.Min, check_bb.Max, GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), true, style.FrameRounding);
		if (*v)
		{
			const float check_sz = ImMin(check_bb.GetWidth(), check_bb.GetHeight());
			const float pad = ImMax(1.0f, (float)(int)(check_sz / 6.0f));
			RenderCheckMark(check_bb.Min + ImVec2(pad, pad), GetColorU32(ImGuiCol_CheckMark), check_bb.GetWidth() - pad*2.0f);
		}

		if (g.LogEnabled)
			LogRenderedText(&text_bb.Min, *v ? "[x]" : "[ ]");
		if (label_size.x > 0.0f)
			RenderText(text_bb.Min, label);

		return pressed;
	}


	bool ToolbarButton(ImTextureID texture, const ImVec4& bg_color, const char* tooltip)
	{
		auto frame_padding = ImGui::GetStyle().FramePadding;
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, frame_padding);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);

		bool ret = false;
		ImGui::SameLine();
		ImVec4 tint_color = ImGui::GetStyle().Colors[ImGuiCol_Text];
		if (ImGui::ImageButton(texture, ImVec2(24, 24), ImVec2(0, 0), ImVec2(1, 1), -1, bg_color, tint_color))
		{
			ret = true;
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("%s", tooltip);
		}
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(3);
		return ret;
	}



	bool BeginToolbar(const char* str_id, ImVec2 screen_pos, ImVec2 size)
	{
		bool is_global = GImGui->CurrentWindowStack.Size == 1;
		SetNextWindowPos(screen_pos);
		ImVec2 frame_padding = GetStyle().FramePadding;
		PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		PushStyleVar(ImGuiStyleVar_WindowPadding, frame_padding);
		PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
		float padding = frame_padding.y * 2;
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
		if (size.x == 0) size.x = GetContentRegionAvailWidth();
		SetNextWindowSize(size);

		bool ret = is_global ? Begin(str_id, nullptr, flags) : BeginChild(str_id, size, false, flags);
		PopStyleVar(3);

		return ret;
	}


	void EndToolbar()
	{
		auto frame_padding = ImGui::GetStyle().FramePadding;
		PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		PushStyleVar(ImGuiStyleVar_WindowPadding, frame_padding);
		PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
		ImVec2 pos = GetWindowPos();
		ImVec2 size = GetWindowSize();
		if (GImGui->CurrentWindowStack.Size == 2) End(); else EndChild();
		PopStyleVar(3);
		ImGuiWindow* win = GetCurrentWindowRead();
		if (GImGui->CurrentWindowStack.Size > 1) SetCursorScreenPos(pos + ImVec2(0, size.y + GetStyle().FramePadding.y * 2));
	}


	//ImVec2 GetOsImePosRequest()
	//{
	//	return ImGui::GetCurrentContext()->OsImePosRequest;
	//}


	void ResetActiveID()
	{
		SetActiveID(0, nullptr);
	}



	static inline bool IsWindowContentHoverableEx(ImGuiWindow* window, ImGuiHoveredFlags flags)
	{
		// An active popup disable hovering on other windows (apart from its own children)
		// FIXME-OPT: This could be cached/stored within the window.
		ImGuiContext& g = *GImGui;
		if (g.NavWindow)
			if (ImGuiWindow* focused_root_window = g.NavWindow->RootWindow)
				if (focused_root_window->WasActive && focused_root_window != window->RootWindow)
				{
					// For the purpose of those flags we differentiate "standard popup" from "modal popup"
					// NB: The order of those two tests is important because Modal windows are also Popups.
					if (focused_root_window->Flags & ImGuiWindowFlags_Modal)
						return false;
					if ((focused_root_window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiHoveredFlags_AllowWhenBlockedByPopup))
						return false;
				}

		return true;
	}


	static bool IsHovered(const ImRect& bb, ImGuiID id)
	{
		ImGuiContext& g = *GImGui;
		if (g.HoveredId == 0 || g.HoveredId == id || g.HoveredIdAllowOverlap)
		{
			ImGuiWindow* window = GetCurrentWindowRead();
			if (g.HoveredWindow == window)
				if ((g.ActiveId == 0 || g.ActiveId == id || g.ActiveIdAllowOverlap) && IsMouseHoveringRect(bb.Min, bb.Max))
					if (IsWindowContentHoverableEx(g.HoveredRootWindow, 0))
						return true;
		}
		return false;
	}


	int PlotHistogramEx(const char* label,
		float(*values_getter)(void* data, int idx),
		void* data,
		int values_count,
		int values_offset,
		const char* overlay_text,
		float scale_min,
		float scale_max,
		ImVec2 graph_size,
		int selected_index)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems) return -1;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		if (graph_size.x == 0.0f) graph_size.x = CalcItemWidth() + (style.FramePadding.x * 2);
		if (graph_size.y == 0.0f) graph_size.y = label_size.y + (style.FramePadding.y * 2);

		const ImRect frame_bb(
			window->DC.CursorPos, window->DC.CursorPos + ImVec2(graph_size.x, graph_size.y));
		const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
		const ImRect total_bb(frame_bb.Min,
			frame_bb.Max +
			ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0));
		ItemSize(total_bb, style.FramePadding.y);
		if (!ItemAdd(total_bb, 0)) return -1;

		// Determine scale from values if not specified
		if (scale_min == FLT_MAX || scale_max == FLT_MAX)
		{
			float v_min = FLT_MAX;
			float v_max = -FLT_MAX;
			for (int i = 0; i < values_count; i++)
			{
				const float v = values_getter(data, i);
				v_min = ImMin(v_min, v);
				v_max = ImMax(v_max, v);
			}
			if (scale_min == FLT_MAX) scale_min = v_min;
			if (scale_max == FLT_MAX) scale_max = v_max;
		}

		RenderFrame(
			frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

		int res_w = ImMin((int)graph_size.x, values_count);

		// Tooltip on hover
		int v_hovered = -1;
		if (IsHovered(inner_bb, 0))
		{
			const float t = ImClamp(
				(g.IO.MousePos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x), 0.0f, 0.9999f);
			const int v_idx = (int)(t * (values_count + 0));
			IM_ASSERT(v_idx >= 0 && v_idx < values_count);

			const float v0 = values_getter(data, (v_idx + values_offset) % values_count);
			SetTooltip("%d: %8.4g", v_idx, v0);
			v_hovered = v_idx;
		}

		const float t_step = 1.0f / (float)res_w;

		float v0 = values_getter(data, (0 + values_offset) % values_count);
		float t0 = 0.0f;
		ImVec2 p0 = ImVec2(t0, 1.0f - ImSaturate((v0 - scale_min) / (scale_max - scale_min)));

		const ImU32 col_base = GetColorU32(ImGuiCol_PlotHistogram);
		const ImU32 col_hovered = GetColorU32(ImGuiCol_PlotHistogramHovered);

		for (int n = 0; n < res_w; n++)
		{
			const float t1 = t0 + t_step;
			const int v_idx = (int)(t0 * values_count + 0.5f);
			IM_ASSERT(v_idx >= 0 && v_idx < values_count);
			const float v1 = values_getter(data, (v_idx + values_offset + 1) % values_count);
			const ImVec2 p1 = ImVec2(t1, 1.0f - ImSaturate((v1 - scale_min) / (scale_max - scale_min)));

			window->DrawList->AddRectFilled(ImLerp(inner_bb.Min, inner_bb.Max, p0),
				ImLerp(inner_bb.Min, inner_bb.Max, ImVec2(p1.x, 1.0f)) + ImVec2(-1, 0),
				selected_index == v_idx ? col_hovered : col_base);

			t0 = t1;
			p0 = p1;
		}

		if (overlay_text)
		{
			RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y),
				frame_bb.Max,
				overlay_text,
				NULL,
				NULL,
				ImVec2(0.5f, 0.5f));
		}

		RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);

		if (v_hovered >= 0 && IsMouseClicked(0))
		{
			return v_hovered;
		}
		return -1;
	}


	bool ListBox(const char* label,
		int* current_item,
		int scroll_to_item,
		bool(*items_getter)(void*, int, const char**),
		void* data,
		int items_count,
		int height_in_items)
	{
		if (!ListBoxHeader(label, items_count, height_in_items)) return false;

		// Assume all items have even height (= 1 line of text). If you need items of different or
		// variable sizes you can create a custom version of ListBox() in your code without using the
		// clipper.
		bool value_changed = false;
		if (scroll_to_item != -1)
		{
			SetScrollY(scroll_to_item * GetTextLineHeightWithSpacing());
		}
		ImGuiListClipper clipper(items_count, GetTextLineHeightWithSpacing());
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
		{
			const bool item_selected = (i == *current_item);
			const char* item_text;
			if (!items_getter(data, i, &item_text)) item_text = "*Unknown item*";

			PushID(i);
			if (Selectable(item_text, item_selected))
			{
				*current_item = i;
				value_changed = true;
			}
			PopID();
		}
		clipper.End();
		ListBoxFooter();
		return value_changed;
	}


	static bool isPredecessor(ImGuiWindow* win, ImGuiWindow* predecessor)
	{
		if (!win) return false;
		if (win == predecessor) return true;
		return isPredecessor(win->ParentWindow, predecessor);
	}


	bool IsFocusedHierarchy()
	{
		ImGuiContext& g = *GImGui;
		return isPredecessor(g.CurrentWindow, g.NavWindow) || isPredecessor(g.NavWindow, g.CurrentWindow);
	}


	void BringToFront()
	{
		ImGuiContext& g = *GImGui;

		ImGuiWindow* window = GImGui->CurrentWindow;

		if ((window->Flags & ImGuiWindowFlags_NoBringToFrontOnFocus) || g.Windows.back() == window)
		{
			return;
		}
		for (int i = 0; i < g.Windows.Size; i++)
		{
			if (g.Windows[i] == window)
			{
				g.Windows.erase(g.Windows.begin() + i);
				break;
			}
		}
		g.Windows.push_back(window);
	}


	static ImVec2 node_pos;
	static ImGuiID last_node_id;


	void BeginNode(ImGuiID id, ImVec2 screen_pos)
	{
		PushID(id);
		last_node_id = id;
		node_pos = screen_pos;
		SetCursorScreenPos(screen_pos + GetStyle().WindowPadding);
		PushItemWidth(150);
		ImDrawList* draw_list = GetWindowDrawList();
		draw_list->ChannelsSplit(2);
		draw_list->ChannelsSetCurrent(1);
		BeginGroup();
	}


	void EndNode(ImVec2& pos)
	{
		ImDrawList* draw_list = GetWindowDrawList();
		ImGui::SameLine();
		float width = GetCursorScreenPos().x - node_pos.x;
		EndGroup();
		PopItemWidth();
		float height = GetCursorScreenPos().y - node_pos.y;
		ImVec2 size(width + GetStyle().WindowPadding.x, height + GetStyle().WindowPadding.y);
		SetCursorScreenPos(node_pos);

		SetNextWindowPos(node_pos);
		SetNextWindowSize(size);
		BeginChild((ImGuiID)last_node_id, size, false, ImGuiWindowFlags_NoInputs);
		EndChild();

		SetCursorScreenPos(node_pos);
		InvisibleButton("bg", size);
		if (IsItemActive() && IsMouseDragging(0))
		{
			pos += GetIO().MouseDelta;
		}

		draw_list->ChannelsSetCurrent(0);
		draw_list->AddRectFilled(node_pos, node_pos + size, ImColor(230, 230, 230), 4.0f);
		draw_list->AddRect(node_pos, node_pos + size, ImColor(150, 150, 150), 4.0f);

		PopID();
		draw_list->ChannelsMerge();
	}


	ImVec2 GetNodeInputPos(ImGuiID id, int input)
	{
		PushID(id);

		ImGuiWindow* parent_win = GetCurrentWindow();
		char title[256];
		ImFormatString(title, IM_ARRAYSIZE(title), "%s.child_%08x", parent_win->Name, id);
		ImGuiWindow* win = FindWindowByName(title);
		if (!win)
		{
			PopID();
			return ImVec2(0, 0);
		}

		ImVec2 pos = win->Pos;
		pos.x -= NODE_SLOT_RADIUS;
		ImGuiStyle& style = GetStyle();
		pos.y += (GetTextLineHeight() + style.ItemSpacing.y) * input;
		pos.y += style.WindowPadding.y + GetTextLineHeight() * 0.5f;


		PopID();
		return pos;
	}


	ImVec2 GetNodeOutputPos(ImGuiID id, int output)
	{
		PushID(id);

		ImGuiWindow* parent_win = GetCurrentWindow();
		char title[256];
		ImFormatString(title, IM_ARRAYSIZE(title), "%s.child_%08x", parent_win->Name, id);
		ImGuiWindow* win = FindWindowByName(title);
		if (!win)
		{
			PopID();
			return ImVec2(0, 0);
		}

		ImVec2 pos = win->Pos;
		pos.x += win->Size.x + NODE_SLOT_RADIUS;
		ImGuiStyle& style = GetStyle();
		pos.y += (GetTextLineHeight() + style.ItemSpacing.y) * output;
		pos.y += style.WindowPadding.y + GetTextLineHeight() * 0.5f;

		PopID();
		return pos;
	}


	bool NodePin(ImGuiID id, ImVec2 screen_pos)
	{
		ImDrawList* draw_list = GetWindowDrawList();
		SetCursorScreenPos(screen_pos - ImVec2(NODE_SLOT_RADIUS, NODE_SLOT_RADIUS));
		PushID(id);
		InvisibleButton("", ImVec2(2 * NODE_SLOT_RADIUS, 2 * NODE_SLOT_RADIUS));
		bool hovered = IsItemHovered();
		PopID();
		draw_list->AddCircleFilled(screen_pos,
			NODE_SLOT_RADIUS,
			hovered ? ImColor(0, 150, 0, 150) : ImColor(150, 150, 150, 150));
		return hovered;
	}


	void NodeLink(ImVec2 from, ImVec2 to)
	{
		ImVec2 p1 = from;
		ImVec2 t1 = ImVec2(+80.0f, 0.0f);
		ImVec2 p2 = to;
		ImVec2 t2 = ImVec2(+80.0f, 0.0f);
		const int STEPS = 12;
		ImDrawList* draw_list = GetWindowDrawList();
		for (int step = 0; step <= STEPS; step++)
		{
			float t = (float)step / (float)STEPS;
			float h1 = +2 * t * t * t - 3 * t * t + 1.0f;
			float h2 = -2 * t * t * t + 3 * t * t;
			float h3 = t * t * t - 2 * t * t + t;
			float h4 = t * t * t - t * t;
			draw_list->PathLineTo(ImVec2(h1 * p1.x + h2 * p2.x + h3 * t1.x + h4 * t2.x,
				h1 * p1.y + h2 * p2.y + h3 * t1.y + h4 * t2.y));
		}
		draw_list->PathStroke(ImColor(200, 200, 100), false, 3.0f);
	}


	ImVec2 operator*(float f, const ImVec2& v)
	{
		return ImVec2(f * v.x, f * v.y);
	}

	static std::string s_LeftTangent = "Left Tangent";
	static std::string s_RightTangent = "Right Tangent";
	static std::string s_BothTangent = "Both Tangent";
	static std::string s_Free = "Free";
	static std::string s_Linear = "Linear";
	static std::string s_Constant = "Constant";
	static std::string s_Smooth = "Smooth";
	static std::string s_Broken = "Broken";
	static std::string s_Auto = "Auto";
	static std::string s_Fast = "Fast";
	static std::string s_Slow = "Slow";
	static std::string s_EditKey = "Edit Key";


	std::map<ImU32, bool> pointsActiveMap;

	struct PointFloatInfo
	{
		bool   isActive;
		ImVec2 pos;

		PointFloatInfo(bool b, const ImVec2& p)
			: isActive(b),
			  pos(p)
		{

		}
	};
	std::map<ImU32, PointFloatInfo> pointsFloatInputMap;

	struct CurvePointID
	{
		size_t hashx;
		size_t hashy;
		int	iCurve;
		int iPoint;

		CurvePointID(const size_t hashX, const size_t& hashY, const int& curveID, const int& pointId)
			: hashx(hashX),
			  hashy(hashY),
			  iCurve(curveID),
			  iPoint(pointId)
		{

		}
	};

	const ImVec2 MaxTangent((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());

	bool IsMaxTangent(const ImVec2& tangent)
	{
		if (tangent.x == (std::numeric_limits<float>::max)() ||
			tangent.y == (std::numeric_limits<float>::max)())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	enum TangentOpMode
	{
		TOM_SMOOTH = 0,
		TOM_BROKEN,
		TOM_AUTO,
		TOM_FAST,
		TOM_SLOW,
	};

	enum TANMODE
	{
		TM_FREE = 0,
		TM_LINEAR,
		TM_CONSTANT,
	};

    enum FitScreen
    {
        None,
        Vertical,
        Horizontal
    };

	enum TangentMode
	{
		Default,
		Free,
		Linear,
		Fast,
		Smooth,
		Slow,
		Constant
	};

    struct CmpStr
    {
        bool operator()(char const *a, char const *b) const
        {
            return std::strcmp(a, b) < 0;
        }
    };

    enum class StorageValues : ImGuiID
    {
        FROM_X = 100,
        FROM_Y,
        WIDTH,
        HEIGHT,
        CURVE_SIZE,
        IS_PANNING,
        POINT_START_X,
        POINT_START_Y,
        SCALE_FACTOR
    };

    std::map<ImU32, TangentOpMode> AllPointsTOM;
    std::map<const char*, std::vector<std::pair<int, ImVec2>>, CmpStr> selectPoints;
    std::map<const char*, FitScreen, CmpStr> fitScreenFlag;
	std::map<const char*, TangentMode, CmpStr> tangentModeFlag;

	static bool mouseDragging = false;
    static float mouseDownX = INT_MIN;
    static float mouseDownY = INT_MIN;

    std::pair<int, ImVec2> CurveEditorGetSingleSelect(const char* label)
    {
        if (selectPoints.find(label) == selectPoints.end())
        {
            return std::make_pair(-1,  ImVec2(INT_MIN, INT_MIN));
        }

        const auto &selectVector = selectPoints[label];
        if (selectVector.size() == 1)
        {
            return selectVector[0];
        }
        else
        {
            return std::make_pair(-1, ImVec2(INT_MIN, INT_MIN));
        }
    }

    void CurveEditorExitSelect(const char* label)
    {
        if (selectPoints.find(label) == selectPoints.end())
        {
            return;
        }

        selectPoints.erase(label);
    }

    void CurveEditorFitVertical(const char* label)
    {
        fitScreenFlag[label] = Vertical;
    }

    void CurveEditorFitHorizontal(const char* label)
    {
        fitScreenFlag[label] = Horizontal;
    }

	void CurveEditorTangent(const char* label, const char *tangentMode)
    {
		TangentMode mode;
		if (std::strcmp(tangentMode, s_Free.c_str()) == 0)  mode = Free;
		else if (std::strcmp(tangentMode, s_Smooth.c_str()) == 0) mode = Smooth;
		else if (std::strcmp(tangentMode, s_Linear.c_str()) == 0) mode = Linear;
		else if (std::strcmp(tangentMode, s_Fast.c_str()) == 0) mode = Fast;
		else if (std::strcmp(tangentMode, s_Slow.c_str()) == 0) mode = Slow;
		else if (std::strcmp(tangentMode, s_Constant.c_str()) == 0) mode = Constant;
		else mode = Default;
		tangentModeFlag[label] = mode;
    }

	inline ImVec4 FindMaxMinPointVec4(std::vector<std::vector<ImVec2>>& values, std::vector<int>& size, ImU32 flags)
	{
		float minX = 0, minY = 0, maxX = 0, maxY = 0;
		for (int n = size.size(), j = 0; j < n; j++)
		{
			for (auto pointIdx = 0; pointIdx < size[j]; ++pointIdx)
			{
				const auto point = flags & static_cast<int>(CurveEditorFlags::NO_TANGENTS)
					                   ? values[j][pointIdx]
					                   : values[j][1 + pointIdx * 3];
				if (pointIdx == 0 && j == 0)
				{
					minX = maxX = point.x;
					minY = maxY = point.y;
				}
				minX = fmin(minX, point.x);
				minY = fmin(minY, point.y);
				maxX = fmax(maxX, point.x);
				maxY = fmax(maxY, point.y);
			}
		}
		return {minX, minY, maxX, maxY};
	}

	void DeletePoint(const ImU32 delHash)
	{
		auto itr = pointsActiveMap.find(delHash);
		if (itr != pointsActiveMap.cend())
		{
			pointsActiveMap.erase(itr);
		}

		auto itr2 = pointsFloatInputMap.find(delHash);
		if (itr2 != pointsFloatInputMap.cend())
		{
			pointsFloatInputMap.erase(itr2);
		}

		auto itr3 = AllPointsTOM.find(delHash);
		if (itr3 != AllPointsTOM.cend())
		{
			AllPointsTOM.erase(itr3);
		}
	}

	void ReplacePrevPoint(const ImU32 prevHash, const ImU32 newHash)
	{
		auto itr = pointsActiveMap.find(prevHash);
		if (itr != pointsActiveMap.cend())
		{
			pointsActiveMap.insert(std::make_pair(newHash, itr->second));
			pointsActiveMap.erase(itr);
		}

		auto itr2 = pointsFloatInputMap.find(prevHash);
		if (itr2 != pointsFloatInputMap.cend())
		{
			pointsFloatInputMap.insert(std::make_pair(newHash, itr2->second));
			pointsFloatInputMap.erase(itr2);
		}

		auto itr3 = AllPointsTOM.find(prevHash);
		if (itr3 != AllPointsTOM.cend())
		{
			AllPointsTOM.insert(std::make_pair(newHash, itr3->second));
			AllPointsTOM.erase(itr3);
		}
	}

	float ClampFloatTangent(const ImVec2& prevKey, const ImVec2& curKey, const ImVec2& nextKey)
	{
		const float PrevToNextTimeDiff = ImMax(0.0000001f, nextKey.x - prevKey.x);
		const float PrevToCurTimeDiff  = ImMax(0.0000001f, curKey.x - prevKey.x);
		const float CurToNextTimeDiff  = ImMax(0.0000001f, nextKey.x - curKey.x);

		float OutTangentVal = 0.0f;

		const float PrevToNextHeightDiff = nextKey.y - prevKey.y;
		const float PrevToCurHeightDiff = curKey.y - prevKey.y;
		const float CurToNextHeightDiff = nextKey.y - curKey.y;

		// Check to see if the current point is crest
		if ((PrevToCurHeightDiff >= 0.0f && CurToNextHeightDiff <= 0.0f) ||
			(PrevToCurHeightDiff <= 0.0f && CurToNextHeightDiff >= 0.0f))
		{
			// Neighbor points are both both on the same side, so zero out the tangent
			OutTangentVal = 0.0f;
		}
		else
		{
			// The three points form a slope

			// Constants
			const float ClampThreshold = 0.333f;

			// Compute height deltas
			const float CurToNextTangent = CurToNextHeightDiff / CurToNextTimeDiff;
			const float PrevToCurTangent = PrevToCurHeightDiff / PrevToCurTimeDiff;
			const float PrevToNextTangent = PrevToNextHeightDiff / PrevToNextTimeDiff;

			// Default to not clamping
			const float UnclampedTangent = PrevToNextTangent;
			float ClampedTangent = UnclampedTangent;

			const float LowerClampThreshold = ClampThreshold;
			const float UpperClampThreshold = 1.0f - ClampThreshold;

			// @todo: Would we get better results using percentange of TIME instead of HEIGHT?
			const float CurHeightAlpha = PrevToCurHeightDiff / PrevToNextHeightDiff;

			if (PrevToNextHeightDiff > 0.0f)
			{
				if (CurHeightAlpha < LowerClampThreshold)
				{
					// 1.0 = maximum clamping (flat), 0.0 = minimal clamping (don't touch)
					const float ClampAlpha = 1.0f - CurHeightAlpha / ClampThreshold;
					const float LowerClamp = ImLerp(PrevToNextTangent, PrevToCurTangent, ClampAlpha);
					ClampedTangent = ImMin(ClampedTangent, LowerClamp);
				}

				if (CurHeightAlpha > UpperClampThreshold)
				{
					// 1.0 = maximum clamping (flat), 0.0 = minimal clamping (don't touch)
					const float ClampAlpha = (CurHeightAlpha - UpperClampThreshold) / ClampThreshold;
					const float UpperClamp = ImLerp(PrevToNextTangent, CurToNextTangent, ClampAlpha);
					ClampedTangent = ImMin(ClampedTangent, UpperClamp);
				}
			}
			else
			{

				if (CurHeightAlpha < LowerClampThreshold)
				{
					// 1.0 = maximum clamping (flat), 0.0 = minimal clamping (don't touch)
					const float ClampAlpha = 1.0f - CurHeightAlpha / ClampThreshold;
					const float LowerClamp = ImLerp(PrevToNextTangent, PrevToCurTangent, ClampAlpha);
					ClampedTangent = ImMax(ClampedTangent, LowerClamp);
				}

				if (CurHeightAlpha > UpperClampThreshold)
				{
					// 1.0 = maximum clamping (flat), 0.0 = minimal clamping (don't touch)
					const float ClampAlpha = (CurHeightAlpha - UpperClampThreshold) / ClampThreshold;
					const float UpperClamp = ImLerp(PrevToNextTangent, CurToNextTangent, ClampAlpha);
					ClampedTangent = ImMax(ClampedTangent, UpperClamp);
				}
			}

			OutTangentVal = ClampedTangent;
		}

		return OutTangentVal;
	}

	ImVec2 normalized(const ImVec2& v)
	{
		float len = sqrtf(v.x *v.x + v.y * v.y);
		if (len < 0.00001) return v;
		float vlen = 1.0f / len;
		return ImVec2(v.x * vlen, v.y * vlen);
	};

	void CalcTangentVector(const ImVec2& prevKey, const ImVec2& curKey, const ImVec2& nextKey, ImVec2& curLeftTangent, ImVec2& curRightTangent)
	{
		float clampTan = ClampFloatTangent(prevKey, curKey, nextKey);

		ImVec2 new_lt(-1, -clampTan);
		normalized(new_lt);
		ImVec2 new_rt(1, clampTan);
		normalized(new_rt);

		float cur_lefttan = curLeftTangent.y / curLeftTangent.x;
		ImVec2 cur_lt(-1, -cur_lefttan);
		normalized(cur_lt);
		float cur_righttan = curRightTangent.y / curRightTangent.x;
		ImVec2 cur_rt(1, cur_righttan);
		normalized(cur_rt);

		float cur_ls = curLeftTangent.x / cur_lt.x;
		float cur_rs = curRightTangent.x / cur_rt.x;

		curLeftTangent = ImVec2(cur_ls * new_lt.x, cur_ls * new_lt.y);
		curRightTangent = ImVec2(cur_rs * new_rt.x, cur_rs * new_rt.y);
	}

	void CalcLeftTangentVectorFromSlope(const float slope, ImVec2& curLeftTangent)
	{
		ImVec2 new_lt(-1, -slope);
		normalized(new_lt);
		float cur_lefttan = curLeftTangent.y / curLeftTangent.x;
		ImVec2 cur_lt(-1, -cur_lefttan);
		normalized(cur_lt);
		float cur_ls = curLeftTangent.x / cur_lt.x;
		curLeftTangent = ImVec2(cur_ls * new_lt.x, cur_ls * new_lt.y);
	}

	void CalcRightTangentVectorFromSlope(const float slope, ImVec2& curRightTangent)
	{
		ImVec2 new_rt(1, slope);
		normalized(new_rt);

		float cur_righttan = curRightTangent.y / curRightTangent.x;
		ImVec2 cur_rt(1, cur_righttan);
		normalized(cur_rt);
		float cur_rs = curRightTangent.x / cur_rt.x;
		curRightTangent = ImVec2(cur_rs * new_rt.x, cur_rs * new_rt.y);
	}

	void ProcessLeftTangent_FastMode(const ImVec2& p, const ImVec2& p_prev, ImVec2& curLeftTangent)
	{
		ImVec2 left_lower_midp = ImVec2(0.5f * (p.x + p_prev.x), ImMin(p_prev.y, p.y));
		ImVec2 left_upper_midp = ImVec2(0.5f * (p.x + p_prev.x), ImMax(p_prev.y, p.y));

		float left_tangent;
		if (p.y >= p_prev.y)
		{
			left_tangent = (p.y - left_lower_midp.y) / (p.x - left_lower_midp.x);
		}
		else
		{
			left_tangent = (p.y - left_upper_midp.y) / (p.x - left_upper_midp.x);
		}

		CalcLeftTangentVectorFromSlope(left_tangent, curLeftTangent);
	}

	void ProcessRightTangent_FastMode(const ImVec2& p, const ImVec2& next_p, ImVec2& curRightTangent)
	{
		ImVec2 right_lower_midp = ImVec2(0.5f * (p.x + next_p.x), ImMin(p.y, next_p.y));
		ImVec2 right_upper_midp = ImVec2(0.5f * (p.x + next_p.x), ImMax(p.y, next_p.y));

		float right_tangent;
		if (p.y >= next_p.y)
		{
			right_tangent = (right_lower_midp.y - p.y) / (right_lower_midp.x - p.x);
		}
		else
		{
			right_tangent = (right_upper_midp.y - p.y) / (right_upper_midp.x - p.x);
		}
		CalcRightTangentVectorFromSlope(right_tangent, curRightTangent);
	}

	std::vector<int> CurveEditor(const char* label
		, std::vector<std::vector<ImVec2>>& values
		, std::vector<int>& points_count
		, std::vector<int>& new_count
		, std::vector<int>& coloridxes
		, std::vector<std::vector<TangentModes>>& tangentsMode
		, const ImVec2& editor_size
		, const ImVec4& range
		, ImU32 flags
		, bool edit)
    {
		const float HEIGHT = 100;
		static ImVec2 start_pan;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		ImVec2 size = editor_size;
		size.x = size.x < 0 ? CalcItemWidth() + (style.FramePadding.x * 2) : size.x;
		size.y = size.y < 0 ? HEIGHT : size.y;

        if (selectPoints.find(label) == selectPoints.end())
        {
            selectPoints[label] = std::vector<std::pair<int, ImVec2>>();
        }

		const auto &curSelectPoint = CurveEditorGetSingleSelect(label);

        const auto frameSelectPoint = selectPoints[label];

        std::vector<std::pair<int, ImVec2>> toBeSelectedVec2s;

		ImGuiWindow* parent_window = GetCurrentWindow();
		ImGuiID id = parent_window->GetID(label);
		for (int i = 0; i < new_count.size(); i++)
		{
			new_count[i] = points_count[i];
		}
		if (!BeginChildFrame(id, size, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{
			EndChildFrame();
			return std::vector<int>(points_count.size(), -1);
		}

		int hovered_idx = -1;
		int hovered_crv = -1;

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
		{
			EndChildFrame();
			return std::vector<int>(points_count.size(), -1);
		}

        if (ImGui::IsHovered(window->WorkRect, window->ID) && IsMouseClicked(0))
        {
            selectPoints[label].clear();
        }

        auto curveSize = window->StateStorage.GetInt(static_cast<ImGuiID>(StorageValues::CURVE_SIZE), values.size());
		auto curveSizeChange = static_cast<int>(values.size()) != curveSize;
		if (flags & static_cast<int>(CurveEditorFlags::RESET) || curveSizeChange)
		{
			CurveEditorExitSelect(label);
			window->StateStorage.Clear();
		}

        const auto &coord = FindMaxMinPointVec4(values, points_count, flags);
        const auto &inner_bb = window->InnerRect;

        auto width = window->StateStorage.GetFloat(static_cast<ImGuiID>(StorageValues::WIDTH), -1);
        auto height = window->StateStorage.GetFloat(static_cast<ImGuiID>(StorageValues::HEIGHT), -1);
        auto from_x = window->StateStorage.GetFloat(static_cast<ImGuiID>(StorageValues::FROM_X), coord.x);
        auto from_y = window->StateStorage.GetFloat(static_cast<ImGuiID>(StorageValues::FROM_Y), height / 2);
        from_y = height / -2;

        ImGuiWindow* curveWindow = GetCurrentWindow();
        ImVec2 curveWindowPos = curveWindow->Pos;
        ImGui::SetNextWindowPos(curveWindowPos + ImVec2(size.x - 20, 0));
        ImGui::SameLine();
        ImGui::BeginChild("Vertical", ImVec2(20, size.y), false, 0);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0.2f, 0.5f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(0.2f, 0.6f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(0.2f, 0.7f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(0.2f, 0.9f, 0.9f));
        ImGui::VSliderFloat("##Vertical", ImVec2(20, size.y), &height, 50.0, 1.0, "");
        ImGui::PopStyleColor(4);

        ImGui::EndChild();
        ImGui::SetNextWindowPos(curveWindowPos + ImVec2(0, size.y - 20));
        ImGui::SameLine();
        ImGui::BeginChild("Horizonta", ImVec2(size.x, 20), false, 0);
        ImGui::PushItemWidth(size.x);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0.2f, 0.5f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(0.2f, 0.6f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(0.2f, 0.7f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(0.2f, 0.9f, 0.9f));
        ImGui::SliderFloat("##Horizonta", &width, 0.1, 50.0, "");
        ImGui::PopStyleColor(4);
        ImGui::EndChild();

        if (height == -1 || width == -1)
        {
            width = coord.z - coord.x;
            height = width * inner_bb.GetHeight() / inner_bb.GetWidth();
            window->StateStorage.SetFloat(static_cast<ImGuiID>(StorageValues::SCALE_FACTOR), inner_bb.GetWidth() / width);
        }
        else if (fitScreenFlag[label] == Vertical)
        {
            const auto h = abs(coord.w - coord.y);
            if (h >= 1)
            {
                height = h;
                width = height / (inner_bb.GetHeight() / inner_bb.GetWidth());
                window->StateStorage.SetFloat(static_cast<ImGuiID>(StorageValues::SCALE_FACTOR), inner_bb.GetWidth() / width);
            }
        }
        else if (fitScreenFlag[label] == Horizontal)
        {
            const auto w = abs (coord.z - coord.x);
            if (w >= 1)
            {
                width = w;
                height = width * inner_bb.GetHeight() / inner_bb.GetWidth();
                window->StateStorage.SetFloat(static_cast<ImGuiID>(StorageValues::SCALE_FACTOR), inner_bb.GetWidth() / width);
            }
        }

        fitScreenFlag[label] = None;

        window->StateStorage.SetFloat(static_cast<ImGuiID>(StorageValues::FROM_X), from_x);
        window->StateStorage.SetFloat(static_cast<ImGuiID>(StorageValues::FROM_Y), from_y);
        window->StateStorage.SetFloat(static_cast<ImGuiID>(StorageValues::WIDTH), width);
        window->StateStorage.SetFloat(static_cast<ImGuiID>(StorageValues::HEIGHT), height);
        window->StateStorage.SetInt(static_cast<ImGuiID>(StorageValues::CURVE_SIZE), curveSize);
        if (curveSizeChange)
        {
            window->StateStorage.SetInt(static_cast<ImGuiID>(StorageValues::CURVE_SIZE), values.size());
            if (coord.w > (from_y + height))
            {
                height = coord.w - from_y + 10;
                window->StateStorage.SetFloat(static_cast<ImGuiID>(StorageValues::HEIGHT), height);
            }
            if (coord.y < from_y)
            {
                height += from_y - coord.y + 10;
				from_y = coord.y - 10;
                window->StateStorage.SetFloat((ImGuiID)StorageValues::FROM_Y, from_y);
                window->StateStorage.SetFloat((ImGuiID)StorageValues::HEIGHT, height);
            }
        }

		auto transform = [&](const ImVec2& pos) -> ImVec2
		{
			float x = (pos.x - from_x) / width;
			float y = (pos.y - from_y) / height;

			return ImVec2(
				inner_bb.Min.x * (1 - x) + inner_bb.Max.x * x,
				inner_bb.Min.y * y + inner_bb.Max.y * (1 - y)
			);
		};

		auto invTransform = [&](const ImVec2& pos) -> ImVec2
		{
			float x = (pos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x);
			float y = (inner_bb.Max.y - pos.y) / (inner_bb.Max.y - inner_bb.Min.y);

			return ImVec2(
				from_x + width * x,
				from_y + height * y
			);
		};

		if (flags & (int)CurveEditorFlags::SHOW_GRID)
		{
			int exp;
			frexp(width / 5, &exp);
			float step_x = (float)ldexp(1.0, exp);
			int cell_cols = int(width / step_x);

			float x = step_x * int(from_x / step_x);
			for (int i = -1; i < cell_cols + 2; ++i)
			{
				ImVec2 a = transform({ x + i * step_x, from_y });
				ImVec2 b = transform({ x + i * step_x, from_y + height });
				window->DrawList->AddLine(a, b, GetColorU32(ImGuiCol_Border));
				char buf[64];
				if (exp > 0)
				{
					ImFormatString(buf, sizeof(buf), " %d", int(x + i * step_x));
				}
				else
				{
					ImFormatString(buf, sizeof(buf), " %f", x + i * step_x);
				}
				window->DrawList->AddText(b, GetColorU32(ImGuiCol_Text), buf);
			}

			frexp(height / 5, &exp);
			float step_y = (float)ldexp(1.0, exp);
			int cell_rows = int(height / step_y);

			float y = step_y * int(from_y / step_y);
			for (int i = -1; i < cell_rows + 2; ++i)
			{
				ImVec2 a = transform({ from_x, y + i * step_y });
				ImVec2 b = transform({ from_x + width, y + i * step_y });
				window->DrawList->AddLine(a, b, GetColorU32(ImGuiCol_Border));
				char buf[64];
				if (exp > 0)
				{
					ImFormatString(buf, sizeof(buf), " %d", int(y + i * step_y));
				}
				else
				{
					ImFormatString(buf, sizeof(buf), " %.2f", y + i * step_y);
				}
				window->DrawList->AddText(a, GetColorU32(ImGuiCol_Text), buf);
			}
		}

        std::vector<int> changed_idx(points_count.size(), -1);
        if (IsMouseDragging(1) && ImGui::IsHovered(window->WorkRect, window->ID))
        {
            for (int i = 0; i < points_count.size(); i++)
            {
                int selectNum = 0;
                std::vector<ImVec2> temp;
                for (auto point : frameSelectPoint)
                {
                    if (point.first == i)
                    {
                        selectNum++;
                        temp.push_back(point.second);
                    }
                }
                if (selectNum == points_count[i])
                {
                    ImVec2 processPoint;
                    ImVec2 retPoint;
                    ImVec2 drag = ImGui::GetMouseDragDelta(1);
                    float x = drag.x * width / (inner_bb.Max.x - inner_bb.Min.x);
                    float y = drag.y * height / (inner_bb.Max.y - inner_bb.Min.y) * -1;
                    ImVec2 beginPoint = temp[selectNum - 1] + ImVec2(x, y);
                    if (beginPoint.x < 0)
                    {
                        continue;
                    }
                    for (int index = 0; index < selectNum; index++)
                    {
                        processPoint = temp[index];
                        retPoint = processPoint + ImVec2(x, y);
                        values[i][(selectNum * 3) - (2 + index * 3)] = retPoint;
                        toBeSelectedVec2s.push_back(std::pair<int, ImVec2>(i,retPoint));
                    }
                    ResetMouseDragDelta(1);
                    changed_idx[i] = selectNum;
                }
            }
        }

		if (ImGui::GetIO().MouseWheel != 0 && ImGui::IsHovered(window->WorkRect, window->ID))
		{
			float scale = powf(1.1, ImGui::GetIO().MouseWheel);
			float wightScale = (scale - 1.0) * 0.2 + 1.0;
			width *= scale;
			height *= scale;
			window->StateStorage.SetFloat((ImGuiID)StorageValues::WIDTH, width);
			window->StateStorage.SetFloat((ImGuiID)StorageValues::HEIGHT, height);
            window->StateStorage.SetFloat(static_cast<ImGuiID>(StorageValues::SCALE_FACTOR), scale);
        }
		if (ImGui::IsMouseReleased(2))
		{
			window->StateStorage.SetBool((ImGuiID)StorageValues::IS_PANNING, false);
		}
		if (window->StateStorage.GetBool((ImGuiID)StorageValues::IS_PANNING, false))
		{
			ImVec2 drag_offset = ImGui::GetMouseDragDelta(2);
			from_x = start_pan.x;
			from_y = start_pan.y;
			from_x -= drag_offset.x * width / (inner_bb.Max.x - inner_bb.Min.x);
			from_y += drag_offset.y * height / (inner_bb.Max.y - inner_bb.Min.y);
			window->StateStorage.SetFloat((ImGuiID)StorageValues::FROM_X, from_x);
			window->StateStorage.SetFloat((ImGuiID)StorageValues::FROM_Y, from_y);
		}
		//else if (ImGui::IsMouseDragging(2) && ImGui::IsItemHovered())
		else if (ImGui::IsMouseDragging(2) && ImGui::IsHovered(window->WorkRect, window->ID))
		{
			window->StateStorage.SetBool((ImGuiID)StorageValues::IS_PANNING, true);
			start_pan.x = from_x;
			start_pan.y = from_y;
		}

		std::vector<std::vector<ImVec2>> drawedcurves;
		int btnid = 0;
		int itanmode = 0;
		int tangentbtnid = 0;
		for (int i = 0; i < points_count.size(); i++)
		{
			itanmode = 0;
			ImGuiCol coloridx;
			switch (coloridxes[i])
			{
			case 1:coloridx = ImGuiCol_PlotLines_1; break;
			case 2:coloridx = ImGuiCol_PlotLines_2; break;
			case 3:coloridx = ImGuiCol_PlotLines_3; break;
			case 4:coloridx = ImGuiCol_PlotLines_4; break;
			case 5:coloridx = ImGuiCol_PlotLines_5; break;
			case 6:coloridx = ImGuiCol_PlotLines_6; break;
			case 7:coloridx = ImGuiCol_PlotLines_7; break;
			case 8:coloridx = ImGuiCol_PlotLines_8; break;
			case 9:coloridx = ImGuiCol_PlotLines_9; break;
			}
			drawedcurves.push_back(std::vector<ImVec2>());
			for (int point_idx = points_count[i] - 2; point_idx >= 0; --point_idx)
			{

				ImVec2* points;
				auto& tanModes = tangentsMode[i];
				if (flags & (int)CurveEditorFlags::NO_TANGENTS)
				{
					points = &values[i][point_idx];
				}
				else
				{
					points = &values[i][1 + point_idx * 3];
				}

				ImVec2 p_prev = points[0];
				ImVec2 tangent_last;
				ImVec2 tangent;
				ImVec2 p;
				ImVec2 m0, m1;
				if (flags & (int)CurveEditorFlags::NO_TANGENTS)
				{
					p = points[1];
				}
				else
				{
					tangent_last = points[1];
					tangent = points[2];
					p = points[3];

					m0 = ImVec2(0.0f, -tangent_last.y);
					m1 = ImVec2(0.0f, tangent.y);

					tangent = normalized(tangent);
					tangent_last = normalized(tangent_last);
				}


				const static auto kFloatEpsilon = 0.001f;
				auto floatCompare = [&](float float1, float float2) -> bool
				{
					return fabs(float1 - float2) < kFloatEpsilon;
				};


				auto handlePoint = [&](ImVec2& p, int idx,bool firstPoint, TangentModes& tanMode, ImU32 pointHash) -> bool
				{
					if (!edit)
					{
						return false;
					}
					static const float SIZE = 3;

					ImVec2 cursor_pos = GetCursorScreenPos();
					ImVec2 pos = transform(p);

					SetCursorScreenPos(pos - ImVec2(SIZE, SIZE));
					PushID(idx + 1);

					InvisibleButton("", ImVec2(2 * NODE_SLOT_RADIUS, 2 * NODE_SLOT_RADIUS));

                    float thickness = 4.0f;
                    ImU32 col = IsItemActive() || IsItemHovered() ? GetColorU32(ImGuiCol_PlotLinesHovered) : GetColorU32(ImGuiCol_PlotLines);

                    if (mouseDragging && mouseDownX != INT_MIN && mouseDownY != INT_MIN)
                    {
                        const auto mouse = ImGui::GetIO().MousePos;
                        const auto region = ImRect(
                                fmin(mouseDownX, mouse.x),
                                fmin(mouseDownY, mouse.y),
                                fmax(mouseDownX, mouse.x),
                                fmax(mouseDownY, mouse.y));
                        if (region.Contains(pos))
                        {
                            toBeSelectedVec2s.emplace_back(std::make_pair(i, ImVec2(p.x, p.y)));
                        }
                    }

                    window->DrawList->AddCircleFilled(pos, thickness, col);

					if (IsItemHovered())
					{
						hovered_crv = i;
						hovered_idx = firstPoint? point_idx:point_idx +1;
					}

					bool changed = false;

					if (IsItemActive() && IsMouseClicked(0))
					{
						window->StateStorage.SetFloat((ImGuiID)StorageValues::POINT_START_X, pos.x);
						window->StateStorage.SetFloat((ImGuiID)StorageValues::POINT_START_Y, pos.y);
					}

					if (pointsActiveMap.find(pointHash) == pointsActiveMap.end())
					{
						pointsActiveMap[pointHash] = false;
					}

					if (AllPointsTOM.find(pointHash) == AllPointsTOM.end())
					{
						AllPointsTOM[pointHash] = TOM_BROKEN;
					}

					auto pf_itr = pointsFloatInputMap.find(pointHash);
					if (pf_itr != pointsFloatInputMap.cend() && pf_itr->second.isActive)
					{
						bool bFloatOpen = true;
						SetNextWindowPos(pf_itr->second.pos);
						SetNextWindowSize(ImVec2(150.0f, 80.0f));
						SetNextWindowBgAlpha(0.0f);
						if (Begin(" ", &bFloatOpen, (int)ImGuiWindowFlags_NoDecoration))
						{
							float v[2];
							v[0] = p.x;
							v[1] = p.y;

							PushItemWidth(100.0f);
							bool x_changed = InputFloat("time", &v[0], 0.0f, 0.0f, "%.2f", ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_AutoSelectAll);
							bool y_changed = InputFloat("value", &v[1], 0.0f, 0.0f, "%.2f", ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_AutoSelectAll);
							PopItemWidth();

							if (!IsWindowFocused())
							{
								pf_itr->second.isActive = false;
							}
							else
							{
								if (x_changed)
								{
									if (range.x != -1 && v[0] < range.x)
									{
										v[0] = range.x;
									}
									if (range.z != -1 && v[0] > range.z)
									{
										v[0] = range.z;
									}
									p.x = v[0];
									changed = true;
								}

								if (y_changed)
								{
									if (range.y != -1 && v[1] < range.y)
									{
										v[1] = range.y;
									}
									if (range.w != -1 && v[1] > range.w)
									{
										v[1] = range.w;
									}
									p.y = v[1];
									changed = true;
								}
							}
							End();
						}
					}

					if (IsItemHovered() || pointsActiveMap[pointHash])
					{
						if (BeginPopupContextWindow())
						{
							pointsActiveMap[pointHash] = true;

							std::map<std::string, std::vector<std::string>> menuLabels;
							menuLabels[s_LeftTangent] = { s_Free, s_Linear, s_Constant };
							menuLabels[s_RightTangent] = { s_Free, s_Linear, s_Constant };
							menuLabels[s_BothTangent] = { s_Free, s_Linear, s_Constant };

							TangentOpMode& tom = AllPointsTOM[pointHash];

							if (MenuItem(s_EditKey.c_str()))
							{
								if (pf_itr == pointsFloatInputMap.cend())
								{
									pointsFloatInputMap.insert(std::make_pair(pointHash, PointFloatInfo(true, GetMousePos())));
								}
								else
								{
									pf_itr->second.isActive = true;
									pf_itr->second.pos = GetMousePos();
								}
							}
							Separator();
							if (MenuItem(s_Auto.c_str(), "", tom == TOM_AUTO))
							{
								tom = TOM_AUTO;
								tanMode.leftTangent = static_cast<int8_t>(TM_FREE);
								tanMode.rightTangent = static_cast<int8_t>(TM_FREE);
								changed = true;
							}
							else if (MenuItem(s_Smooth.c_str(), "", tom == TOM_SMOOTH))
							{
								tom = TOM_SMOOTH;
								tanMode.leftTangent = static_cast<int8_t>(TM_FREE);
								tanMode.rightTangent = static_cast<int8_t>(TM_FREE);
							}
							else if (MenuItem(s_Broken.c_str(), "", tom == TOM_BROKEN))
							{
								tom = TOM_BROKEN;
								tanMode.leftTangent = static_cast<int8_t>(TM_FREE);
								tanMode.rightTangent = static_cast<int8_t>(TM_FREE);
							}
							else if (MenuItem(s_Fast.c_str(), "", tom == TOM_FAST))
							{
								tom = TOM_FAST;
								tanMode.leftTangent = static_cast<int8_t>(TM_FREE);
								tanMode.rightTangent = static_cast<int8_t>(TM_FREE);
								changed = true;
							}
							else if (MenuItem(s_Slow.c_str(), "", tom == TOM_SLOW))
							{
								tom = TOM_SLOW;
								tanMode.leftTangent = static_cast<int8_t>(TM_FREE);
								tanMode.rightTangent = static_cast<int8_t>(TM_FREE);
								changed = true;
							}
							Separator();

							for (const auto& label : menuLabels)
							{
								if (BeginMenu(label.first.c_str()))
								{
									for (size_t iTangentMode = 0; iTangentMode < label.second.size(); ++iTangentMode)
									{
										bool bSelected = false;
										if (iTangentMode == tanMode.leftTangent &&
											label.first == s_LeftTangent)
										{
											bSelected = true;
										}
										else if (iTangentMode == tanMode.rightTangent &&
											label.first == s_RightTangent)
										{
											bSelected = true;
										}

										if (tom != TOM_BROKEN)
										{
											bSelected = false;
										}

										if (MenuItem(label.second.at(iTangentMode).c_str(), "", bSelected))
										{
											if (label.first == s_LeftTangent)
											{
												tanMode.leftTangent = iTangentMode;
											}
											else if (label.first == s_RightTangent)
											{
												tanMode.rightTangent = iTangentMode;
											}
											else if (label.first == s_BothTangent)
											{
												tanMode.leftTangent = iTangentMode;
												tanMode.rightTangent = iTangentMode;
											}
											AllPointsTOM[pointHash] = TOM_BROKEN;
											changed = true;
										}
									}
									EndMenu();
								}

							}
							EndPopup();
						}
						else
						{
							pointsActiveMap[pointHash] = false;
						}

					}

					const auto &selectPoint = curSelectPoint.second;
					const auto checkTangent = curSelectPoint.first == i && tangentModeFlag[label] != Default;
					if (checkTangent && floatCompare(selectPoint.x, p.x) && floatCompare(selectPoint.y, p.y))
					{
						if (tangentModeFlag[label] == Smooth)
						{
							AllPointsTOM[pointHash] = TOM_SMOOTH;
							tanMode.leftTangent = static_cast<int8_t>(TM_FREE);
							tanMode.rightTangent = static_cast<int8_t>(TM_FREE);
							changed = true;
						}
						else if (tangentModeFlag[label] == Fast)
						{
							AllPointsTOM[pointHash] = TOM_FAST;
							tanMode.leftTangent = static_cast<int8_t>(TM_FREE);
							tanMode.rightTangent = static_cast<int8_t>(TM_FREE);
							changed = true;
						}
						else if (tangentModeFlag[label] == Slow)
						{
							AllPointsTOM[pointHash] = TOM_SLOW;
							tanMode.leftTangent = static_cast<int8_t>(TM_FREE);
							tanMode.rightTangent = static_cast<int8_t>(TM_FREE);
							changed = true;
						}
						else if (tangentModeFlag[label] == Free)
						{
							tanMode.leftTangent = TM_FREE;
							tanMode.rightTangent = TM_FREE;
							AllPointsTOM[pointHash] = TOM_BROKEN;
							changed = true;
						}
						else if (tangentModeFlag[label] == Linear)
						{
							tanMode.leftTangent = TM_LINEAR;
							tanMode.rightTangent = TM_LINEAR;
							AllPointsTOM[pointHash] = TOM_BROKEN;
							changed = true;
						}
						else if (tangentModeFlag[label] == Constant)
						{
							tanMode.leftTangent = TM_CONSTANT;
							tanMode.rightTangent = TM_CONSTANT;
							AllPointsTOM[pointHash] = TOM_BROKEN;
							changed = true;
						}
						tangentModeFlag[label] = Default;
					}

					if (IsItemHovered() || IsItemActive() && IsMouseDragging(0))
					{
						char tmp[64];
						ImFormatString(tmp, sizeof(tmp), "%0.2f, %0.2f", p.x, p.y);
						window->DrawList->AddText({ pos.x, pos.y - GetTextLineHeight() }, GetColorU32(ImGuiCol_Text), tmp);

                        if (IsMouseLeftDown())
                        {
                            toBeSelectedVec2s.emplace_back(std::make_pair(i, ImVec2(p.x, p.y)));
                        }
					}

					if (IsItemActive() && IsMouseDragging(0))
					{
						ImGuiContext& g = *GImGui;
						pos.x = window->StateStorage.GetFloat((ImGuiID)StorageValues::POINT_START_X, pos.x);
						pos.y = window->StateStorage.GetFloat((ImGuiID)StorageValues::POINT_START_Y, pos.y);
						pos += ImGui::GetMouseDragDelta();
						ImVec2 v = invTransform(pos);
						if (range.x != -1 && v.x < range.x)
						{
							v.x = range.x;
						}
						if (range.z != -1 && v.x > range.z)
						{
							v.x = range.z;
						}
						if (range.y != -1 && v.y < range.y)
						{
							v.y = range.y;
						}
						if (range.w != -1 && v.y > range.w)
						{
							v.y = range.w;
						}
						p = v;
						changed = true;

					}
					PopID();

					SetCursorScreenPos(cursor_pos);
					return changed;
				};

				auto handleTangent = [&](ImVec2& t, const ImVec2& p, int idx) -> bool
				{
					if (!edit)
					{
						return false;
					}
					static const float SIZE = 2;
					static const float LENGTH = 48;

					ImVec2 cursor_pos = GetCursorScreenPos();
					ImVec2 pos = transform(p);
					ImVec2 tang = pos + normalized(ImVec2(t.x, -t.y)) * LENGTH;

					SetCursorScreenPos(tang - ImVec2(SIZE, SIZE));
					PushID(-idx);
					InvisibleButton("", ImVec2(2 * NODE_SLOT_RADIUS, 2 * NODE_SLOT_RADIUS));

					window->DrawList->AddLine(pos, tang, GetColorU32(ImGuiCol_PlotLines));

					ImU32 col = IsItemHovered() ? GetColorU32(ImGuiCol_PlotLinesHovered) : GetColorU32(ImGuiCol_PlotLines);

					window->DrawList->AddLine(tang + ImVec2(-SIZE, SIZE), tang + ImVec2(SIZE, SIZE), col);
					window->DrawList->AddLine(tang + ImVec2(SIZE, SIZE), tang + ImVec2(SIZE, -SIZE), col);
					window->DrawList->AddLine(tang + ImVec2(SIZE, -SIZE), tang + ImVec2(-SIZE, -SIZE), col);
					window->DrawList->AddLine(tang + ImVec2(-SIZE, -SIZE), tang + ImVec2(-SIZE, SIZE), col);

					bool changed = false;
					if (IsItemActive() && IsMouseDragging(0))
					{
						tang = GetIO().MousePos - pos;
						tang = normalized(tang);
						tang.y *= -1;

						t = tang;
						changed = true;
					}
					PopID();

					SetCursorScreenPos(cursor_pos);
					return changed;
				};

				PushID((point_idx + 1) * (i + 1));

				if ((flags & (int)CurveEditorFlags::NO_TANGENTS) == 0)
				{
					if (!IsMaxTangent(tangent_last) || !IsMaxTangent(tangent))
					{
						window->DrawList->AddHermiteCurve(
							transform(p_prev),
							m0.y,
							m1.y,
							transform(p),
							GetColorU32(coloridx),
							1.0f,
							drawedcurves[i],
							20);

					}
					tangentbtnid++;
					const auto hasher = std::hash<float>{};
					if (!IsMaxTangent(tangent_last))
					{
						if (handleTangent(tangent_last, p_prev, tangentbtnid))
						{
							points[1] = ImClamp(tangent_last, ImVec2(1e-3, -1), ImVec2(1, 1));
							changed_idx[i] = point_idx;

							CurvePointID prev_pointId = CurvePointID(hasher(p_prev.x), hasher(p_prev.y), i, point_idx);
							ImU32 prev_pointHash = ImHashData(&prev_pointId, sizeof(CurvePointID));
							auto prev_itr = AllPointsTOM.find(prev_pointHash);
							if (prev_itr != AllPointsTOM.cend())
							{
								if (prev_itr->second == TOM_AUTO ||
									prev_itr->second == TOM_FAST ||
									prev_itr->second == TOM_SLOW)
								{
									prev_itr->second = TOM_SMOOTH;
								}

								if (prev_itr->second == TOM_SMOOTH)
								{
									if (point_idx != 0)
									{
										ImVec2* prevPoints = &values[i][1 + (point_idx - 1) * 3];
										float prev_t = prevPoints[2].y / prevPoints[2].x;

										ImVec2 lt(-1.0f, -prev_t);
										lt = normalized(lt);
										float delta_dis = prevPoints[2].x / lt.x;

										lt = ImVec2(-1.0f, -(points[1].y / points[1].x));
										lt = normalized(lt);

										prevPoints[2] = ImVec2(delta_dis * lt.x, delta_dis * lt.y);
									}
								}
							}
						}
					}
					tangentbtnid++;
					if (!IsMaxTangent(tangent))
					{
						if (handleTangent(tangent, p, tangentbtnid))
						{
							points[2] = ImClamp(tangent, ImVec2(-1, -1), ImVec2(-1e-3, 1));
							changed_idx[i] = point_idx + 1;

							CurvePointID p_pointId = CurvePointID(hasher(p.x), hasher(p.y), i, point_idx+1);
							ImU32 p_pointHash = ImHashData(&p_pointId, sizeof(CurvePointID));
							auto p_itr = AllPointsTOM.find(p_pointHash);
							if (p_itr != AllPointsTOM.cend())
							{
								if (p_itr->second == TOM_SMOOTH)
								{
									if (point_idx != points_count[i] - 2)
									{
										ImVec2* nextPoints = &values[i][1 + (point_idx + 1) * 3];
										nextPoints[1] = ImVec2(-points[2].x, -points[2].y);
									}

								}
								else if (p_itr->second == TOM_AUTO ||
									     p_itr->second == TOM_FAST ||
										 p_itr->second == TOM_SLOW)
								{
									p_itr->second = TOM_SMOOTH;
									if (point_idx != points_count[i] - 2)
									{
										ImVec2* nextPoints = &values[i][1 + (point_idx + 1) * 3];
										nextPoints[1] = ImVec2(-points[2].x, -points[2].y);
									}
								}
							}
						}
					}
					btnid++;
					itanmode++;
					size_t iTanMode = points_count[i] - itanmode;
					CurvePointID pointId = CurvePointID(hasher(p.x), hasher(p.y), i, iTanMode);
					ImU32 pointHash = ImHashData(&pointId, sizeof(CurvePointID));

					if (handlePoint(p, btnid, false, tanModes[iTanMode], pointHash))
					{
						if (p.x <= p_prev.x) p.x = p_prev.x + 0.001f;
						if (point_idx < points_count[i] - 2 && p.x >= points[6].x)
						{
							p.x = points[6].x - 0.001f;
						}
						points[3] = p;
						changed_idx[i] = point_idx + 1;

						//process current p tangentmode
						CurvePointID new_pointId = CurvePointID(hasher(p.x), hasher(p.y), i, iTanMode);
						ImU32 new_pointHash = ImHashData(&new_pointId, sizeof(CurvePointID));
						if (new_pointHash != pointHash)
						{
							ReplacePrevPoint(pointHash, new_pointHash);
						}

						auto p_itr = AllPointsTOM.find(new_pointHash);
						if (p_itr != AllPointsTOM.cend())
						{

							if (p_itr->second == TOM_AUTO)
							{
								if (point_idx != points_count[i] - 2)
								{
									ImVec2* nextPoints = &values[i][1 + (point_idx + 1) * 3];
									const ImVec2& nextKey = nextPoints[3];
									CalcTangentVector(p_prev, p, nextKey, points[2], nextPoints[1]);
								}
							}
							else if (p_itr->second == TOM_FAST)
							{
								ProcessLeftTangent_FastMode(p, p_prev, points[2]);

								if (point_idx != points_count[i] - 2)
								{
									ImVec2* next_points = &values[i][1 + (point_idx + 1) * 3];
									const ImVec2& next_p = next_points[3];
									ProcessRightTangent_FastMode(p, next_p, next_points[1]);
								}
							}
							else if (p_itr->second == TOM_SLOW)
							{
								CalcLeftTangentVectorFromSlope(0.0f, points[2]);
								if (point_idx != points_count[i] - 2)
								{
									ImVec2* next_points = &values[i][1 + (point_idx + 1) * 3];
									CalcRightTangentVectorFromSlope(0.0f, next_points[1]);
								}
							}
						}
						//process prev_p tangentmode
						CurvePointID prev_pointId = CurvePointID(hasher(p_prev.x), hasher(p_prev.y), i, iTanMode - 1);
						ImU32 prev_pointHash = ImHashData(&prev_pointId, sizeof(CurvePointID));
						auto prev_itr = AllPointsTOM.find(prev_pointHash);
						if (prev_itr != AllPointsTOM.cend())
						{
							if (prev_itr->second == TOM_AUTO)
							{
								if (point_idx >0)
								{
									ImVec2* prev_points = &values[i][1 + (point_idx - 1) * 3];

									/*tangent_last = points[1];
									tangent = points[2];
									p = points[3];*/

									CalcTangentVector(prev_points[0], prev_points[3], p, prev_points[2], points[1]);
								}
							}
							else if (prev_itr->second == TOM_FAST)
							{
								ProcessRightTangent_FastMode(p_prev, p, points[1]);
							}
						}
						// process next_p tangentmode
						if (point_idx != points_count[i] - 2)
						{
							ImVec2* next_points = &values[i][1 + (point_idx + 1) * 3];
							const ImVec2& next_p = next_points[3];
							CurvePointID next_pointId = CurvePointID(hasher(next_p.x), hasher(next_p.y), i, iTanMode + 1);
							ImU32 next_pointHash = ImHashData(&next_pointId, sizeof(CurvePointID));
							auto next_itr = AllPointsTOM.find(next_pointHash);
							if (next_itr != AllPointsTOM.cend())
							{
								if (next_itr->second == TOM_AUTO)
								{
									if (point_idx + 1 < points_count[i] - 2)
									{
										ImVec2* next_next_points = &values[i][1 + (point_idx + 2) * 3];
										CalcTangentVector(p, next_p, next_next_points[3], next_points[2], next_next_points[1]);
									}
								}
								else if (next_itr->second == TOM_FAST)
								{
									ProcessLeftTangent_FastMode(next_p, p, points[2]);
								}
							}
						}
					}

					if (tanModes[iTanMode].leftTangent == 2)
					{
						if (points[2].x != (std::numeric_limits<float>::max)() &&
							points[2].y != (std::numeric_limits<float>::max)())
						{
							points[2] = MaxTangent;
							changed_idx[i] = point_idx + 1;
						}

					}
					if (tanModes[iTanMode - 1].rightTangent == 2)
					{
						if (points[1].x != (std::numeric_limits<float>::max)() &&
							points[1].y != (std::numeric_limits<float>::max)())
						{
							points[1] = MaxTangent;
							changed_idx[i] = point_idx;
						}
					}

					if (IsMaxTangent(tangent_last) || IsMaxTangent(tangent))
					{
						ImVec2 interPoint(p.x, p_prev.y);
						window->DrawList->AddLine(transform(p_prev), transform(interPoint), GetColorU32(coloridx), 1.0f);
						window->DrawList->AddLine(transform(interPoint), transform(p), GetColorU32(coloridx), 1.0f);
					}
				}
				else
				{
					window->DrawList->AddLine(transform(p_prev), transform(p), GetColorU32(ImGuiCol_PlotLines), 1.0f);
					btnid++;
					itanmode++;
					size_t iTanMode = points_count[i] - itanmode;
					const auto hasher = std::hash<float>{};
					CurvePointID pointId = CurvePointID(hasher(p.x), hasher(p.y), i, iTanMode);
					ImU32 pointHash = ImHashData(&pointId, sizeof(CurvePointID));
					if (handlePoint(p, btnid,false, tanModes[points_count[i] - itanmode], pointHash))
					{
						if (p.x <= p_prev.x) p.x = p_prev.x + 0.001f;
						if (point_idx < points_count[i] - 2 && p.x >= points[2].x)
						{
							p.x = points[2].x - 0.001f;
						}
						points[1] = p;
						changed_idx[i] = point_idx + 1;
					}
				}
				if (point_idx == 0)
				{
					btnid++;
					itanmode++;
					size_t iTanMode = points_count[i] - itanmode;
					const auto hasher = std::hash<float>{};
					CurvePointID pointId = CurvePointID(hasher(p_prev.x), hasher(p_prev.y), i, iTanMode);
					ImU32 pointHash = ImHashData(&pointId, sizeof(CurvePointID));
					if (handlePoint(p_prev, btnid,true, tanModes[points_count[i] - itanmode], pointHash))
					{
						if (p.x <= p_prev.x) p_prev.x = p.x - 0.001f;
						points[0] = p_prev;
						changed_idx[i] = point_idx;

						CurvePointID new_pointId = CurvePointID(hasher(p_prev.x), hasher(p_prev.y), i, iTanMode);
						ImU32 new_pointHash = ImHashData(&new_pointId, sizeof(CurvePointID));
						if (new_pointHash != pointHash)
						{
							ReplacePrevPoint(pointHash, new_pointHash);
						}

						CurvePointID next_pointId = CurvePointID(hasher(p.x), hasher(p.y), i, iTanMode + 1);
						ImU32 next_pointHash = ImHashData(&next_pointId, sizeof(CurvePointID));
						auto next_itr = AllPointsTOM.find(next_pointHash);
						if (next_itr != AllPointsTOM.cend())
						{
							if (next_itr->second == TOM_AUTO)
							{
								if (points_count[i] > 2)
								{
									ImVec2* next_points = &values[i][1 + (point_idx + 1) * 3];
									CalcTangentVector(p_prev, p, next_points[3], points[2], next_points[1]);
								}
							}
							else if (next_itr->second == TOM_FAST)
							{
								ProcessLeftTangent_FastMode(p, p_prev, points[2]);
							}
						}
					}
					auto first_p = AllPointsTOM.find(pointHash);
					if (first_p != AllPointsTOM.cend())
					{
						if (first_p->second == TOM_FAST)
						{
							ProcessRightTangent_FastMode(p_prev, p, points[1]);
						}
						else if (first_p->second == TOM_SLOW)
						{
							CalcRightTangentVectorFromSlope(0.0f, points[1]);
						}
					}
				}
				PopID();
			}
		}
		SetCursorScreenPos(inner_bb.Min);

        if (mouseDragging && mouseDownX != INT_MIN && mouseDownY != INT_MIN)
        {
            ImVec2 a = ImVec2(mouseDownX, mouseDownY);
            ImVec2 b = ImVec2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);

            const auto &diff = b - a;
            if (diff.x * diff.x + diff.y * diff.y >= 4)
            {
                window->DrawList->AddRectFilled(a, b, ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 0.1f)));
                window->DrawList->AddRect(a, b, ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 0.5f)));
            }
        }

        if (ImGui::GetIO().MouseDown[0] && ImGui::IsHovered(window->WorkRect, window->ID))
        {
            mouseDragging = true;
            mouseDownX = ImGui::GetIO().MousePos.x;
            mouseDownY = ImGui::GetIO().MousePos.y;
        }

        if (ImGui::GetIO().MouseReleased[0])
        {
            mouseDragging = false;
            mouseDownX = INT_MIN;
            mouseDownY = INT_MIN;
        }

        if (!toBeSelectedVec2s.empty())
        {
            selectPoints[label].assign(toBeSelectedVec2s.begin(), toBeSelectedVec2s.end());
        }

        for (auto &item: selectPoints[label])
        {
            const auto &pos = transform(item.second);
            window->DrawList->AddCircleFilled(pos, 6.0f, ImGui::ColorConvertFloat4ToU32(ImVec4(1, .15f, .15f, 0.8f)));
            window->DrawList->AddCircleFilled(pos, 4.0f, GetColorU32(ImGuiCol_PlotLines));
        }

		bool IgnoreDoubleClick = false;
		bool ActiveTest = false;

		InvisibleButton("bg", inner_bb.Max - inner_bb.Min);

		if (edit && ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0) && !IgnoreDoubleClick)
		{
			ImVec2 mp = ImGui::GetMousePos();
			ImVec2 new_p = invTransform(mp);

			float minydiff = 9999999;
			int curveidx = 0;
			for (int i=0;i<drawedcurves.size();i++)
			{
				for (int j=0;j<(int)(drawedcurves[i].size())-1;j++)
				{
					ImVec2 cur =	invTransform(drawedcurves[i][j]);
					ImVec2 nxt =  invTransform(drawedcurves[i][j+1]);
					if (cur.x<new_p.x&&nxt.x>new_p.x)
					{
						float percent = (new_p.x - cur.x) / (nxt.x - cur.x);
						float curvey = cur.y + percent*(nxt.y - cur.y);
						float dis = abs(new_p.y - curvey);
						if (dis < minydiff)
						{
							minydiff = dis;
							curveidx = i;
						}
						break;
					}
					if ((j == 0) && (cur.x > new_p.x))
					{
						float dis = abs(new_p.y - cur.y);
						if (dis < minydiff)
						{
							minydiff = dis;
							curveidx = i;
						}
						break;
					}
					if ((j== drawedcurves[i].size() - 2)&&(nxt.x<new_p.x))
					{
						float dis = abs(new_p.y - nxt.y);
						if (dis < minydiff)
						{
							minydiff = dis;
							curveidx = i;
						}
					}
				}

			}

			values[curveidx].push_back(ImVec2());
			values[curveidx].push_back(ImVec2());
			values[curveidx].push_back(ImVec2());
			ImVec2* points = values[curveidx].data();

			bool inTheRange = true;

			if ((range.x != -1 && new_p.x < range.x)
				|| (range.z != -1 && new_p.x > range.z)
				|| (range.y != -1 && new_p.y < range.y)
				|| (range.w != -1 && new_p.y > range.w)
				)
			{
				inTheRange = false;
			}

			if (inTheRange)
			{
				selectPoints[label].emplace_back(std::make_pair(curveidx, ImVec2(new_p.x, new_p.y)));

				if ((flags & (int)CurveEditorFlags::NO_TANGENTS) == 0)
				{
					points[points_count[curveidx] * 3 + 0] = ImVec2(-0.2f, 0);
					points[points_count[curveidx] * 3 + 1] = new_p;
					points[points_count[curveidx] * 3 + 2] = ImVec2(0.2f, 0);;
					++new_count[curveidx];

					auto compare = [](const void* a, const void* b) -> int
					{
						float fa = (((const ImVec2*)a) + 1)->x;
						float fb = (((const ImVec2*)b) + 1)->x;
						return fa < fb ? -1 : (fa > fb) ? 1 : 0;
					};

					qsort(values[curveidx].data(), points_count[curveidx] + 1, sizeof(ImVec2) * 3, compare);

					ImVec2* pFirstPoint = values[curveidx].data() + 1;
					auto& tanModes = tangentsMode[curveidx];
					tanModes.push_back(TangentModes());
					int insert_pos = -1;
					for (size_t ip = 0; ip < new_count[curveidx]; ++ip)
					{
						if (new_p.x < pFirstPoint->x)
						{
							insert_pos = ip;
							break;
						}

						pFirstPoint += 3;
					}

					if (insert_pos != -1)
					{
						for (size_t ip = 0; ip < new_count[curveidx] - insert_pos; ++ip)
						{
							std::swap(tanModes[new_count[curveidx] - 1 - ip], tanModes[new_count[curveidx] - 2 - ip]);
						}
					}
				}
				else
				{
					points[points_count[curveidx]] = new_p;
					++new_count[curveidx];

					auto compare = [](const void* a, const void* b) -> int
					{
						float fa = ((const ImVec2*)a)->x;
						float fb = ((const ImVec2*)b)->x;
						return fa < fb ? -1 : (fa > fb) ? 1 : 0;
					};

					qsort(values[curveidx].data(), points_count[curveidx] + 1, sizeof(ImVec2), compare);
				}

			}

		}

		if (hovered_crv>-1&&hovered_idx >= 0 && ImGui::IsMouseDoubleClicked(0) && new_count[hovered_crv] && points_count[hovered_crv] > 2)
		{
			ImVec2* points = values[hovered_crv].data();
			--new_count[hovered_crv];
			if ((flags & (int)CurveEditorFlags::NO_TANGENTS) == 0)
			{
				int j = hovered_idx * 3;
				const ImVec2& del_p = points[j + 1];
				const auto hasher = std::hash<float>{};
				CurvePointID del_pointId = CurvePointID(hasher(del_p.x), hasher(del_p.y), hovered_crv, hovered_idx);
				ImU32 del_hash = ImHashData(&del_pointId, sizeof(CurvePointID));
				DeletePoint(del_hash);

				for (; j < points_count[hovered_crv] * 3 - 3; j += 3)
				{
					points[j + 0] = points[j + 3];
					points[j + 1] = points[j + 4];
					points[j + 2] = points[j + 5];
				}

				values[hovered_crv].pop_back();
				values[hovered_crv].pop_back();
				values[hovered_crv].pop_back();

				auto& tanModes = tangentsMode[hovered_crv];
				for (size_t ip = hovered_idx; ip < points_count[hovered_crv] - 1; ++ip)
				{
					tanModes[ip] = tanModes[ip + 1];
				}
				tanModes.pop_back();
			}
			else
			{
				for (int j = hovered_idx; j < points_count[hovered_crv] - 1; ++j)
				{
					points[j] = points[j + 1];
				}
			}
			selectPoints[label].clear();
		}


		EndChildFrame();
        const ImRect frameRect(inner_bb.Min - style.FramePadding, inner_bb.Max + style.FramePadding);
        RenderText(ImVec2(frameRect.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);
        return std::move(changed_idx);
	}


	//bool BeginResizablePopup(const char* str_id, const ImVec2& size_on_first_use)
	//{
	//	if (GImGui->OpenPopupStack.Size <= GImGui->CurrentPopupStack.Size)
	//	{
	//		GImGui->NextWindowData.Clear();
	//		return false;
	//	}
	//	ImGuiContext& g = *GImGui;
	//	ImGuiWindow* window = g.CurrentWindow;
	//	const ImGuiID id = window->GetID(str_id);
	//	if (!IsPopupOpen(id))
	//	{
	//		GImGui->NextWindowData.Clear();
	//		return false;
	//	}

	//	ImGuiWindowFlags flags = ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

	//	char name[32];
	//	ImFormatString(name, 20, "##popup_%08x", id);

	//	ImGui::SetNextWindowSize(size_on_first_use, ImGuiCond_FirstUseEver);
	//	bool opened = ImGui::Begin(name, NULL, flags);
	//	if (!opened)
	//		ImGui::EndPopup();

	//	return opened;
	//}


	void IntervalGraph(const unsigned long long* value_pairs,
		int value_pairs_count,
		unsigned long long scale_min,
		unsigned long long scele_max)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems) return;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		ImVec2 graph_size(CalcItemWidth() + (style.FramePadding.x * 2), ImGui::GetTextLineHeight());

		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(graph_size.x, graph_size.y));
		const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
		const ImRect total_bb(frame_bb.Min, frame_bb.Max);
		ItemSize(total_bb, style.FramePadding.y);
		if (!ItemAdd(total_bb, 0)) return;

		double graph_length = double(scele_max - scale_min);
		const ImU32 col_base = GetColorU32(ImGuiCol_PlotHistogram);

		for (int i = 0; i < value_pairs_count; ++i)
		{
			ImVec2 tmp = frame_bb.Min + ImVec2(float((value_pairs[i * 2] - scale_min) / graph_length * graph_size.x), 0);
			window->DrawList->AddRectFilled(
				tmp, tmp + ImVec2(ImMax(1.0f, float(value_pairs[i * 2 + 1] / graph_length * graph_size.x)), graph_size.y), col_base);
		}
	}


	bool LabellessInputText(const char* label, char* buf, size_t buf_size, float width)
	{
		auto pos = GetCursorPos();
		PushItemWidth(width < 0 ? GetContentRegionAvail().x : width);
		char tmp[32];
		strcpy(tmp, "##");
		strcat(tmp, label);
		bool ret = InputText(tmp, buf, buf_size);
		if (buf[0] == 0 && !IsItemActive())
		{
			pos.x += GetStyle().FramePadding.x;
			SetCursorPos(pos);
			AlignTextToFramePadding();
			TextColored(GetStyle().Colors[ImGuiCol_TextDisabled], "%s", label);
		}
		PopItemWidth();
		return ret;
	}


	void Rect(float w, float h, ImU32 color)
	{
		ImGuiWindow* win = GetCurrentWindow();
		ImVec2 screen_pos = GetCursorScreenPos();
		ImVec2 end_pos = screen_pos + ImVec2(w, h);
		ImRect total_bb(screen_pos, end_pos);
		ItemSize(total_bb);
		if (!ItemAdd(total_bb, 0)) return;
		win->DrawList->AddRectFilled(screen_pos, end_pos, color);
	}


	void HSplitter(const char* str_id, ImVec2* size)
	{
		ImVec2 screen_pos = GetCursorScreenPos();
		InvisibleButton(str_id, ImVec2(-1, 3));
		ImVec2 end_pos = screen_pos + GetItemRectSize();
		ImGuiWindow* win = GetCurrentWindow();
		ImVec4* colors = GetStyle().Colors;
		ImU32 color = GetColorU32(IsItemActive() || IsItemHovered() ? colors[ImGuiCol_ButtonActive] : colors[ImGuiCol_Button]);
		win->DrawList->AddRectFilled(screen_pos, end_pos, color);
		if (ImGui::IsItemActive())
		{
			size->y = ImMax(1.0f, ImGui::GetIO().MouseDelta.y + size->y);
		}
	}


	void VSplitter(const char* str_id, ImVec2* size)
	{
		ImVec2 screen_pos = GetCursorScreenPos();
		InvisibleButton(str_id, ImVec2(3, -1));
		ImVec2 end_pos = screen_pos + GetItemRectSize();
		ImGuiWindow* win = GetCurrentWindow();
		ImVec4* colors = GetStyle().Colors;
		ImU32 color = GetColorU32(IsItemActive() || IsItemHovered() ? colors[ImGuiCol_ButtonActive] : colors[ImGuiCol_Button]);
		win->DrawList->AddRectFilled(screen_pos, end_pos, color);
		if (ImGui::IsItemActive())
		{
			size->x = ImMax(1.0f, ImGui::GetIO().MouseDelta.x + size->x);
		}
	}

	static float s_max_timeline_value;


	bool BeginTimeline(const char* str_id, float max_value)
	{
		s_max_timeline_value = max_value;
		return BeginChild(str_id);
	}


	static const float TIMELINE_RADIUS = 6;


	bool TimelineEvent(const char* str_id, float* value)
	{
		ImGuiWindow* win = GetCurrentWindow();
		const ImU32 inactive_color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_Button]);
		const ImU32 active_color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_ButtonHovered]);
		const ImU32 line_color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_Text]);
		bool changed = false;
		ImVec2 cursor_pos = win->DC.CursorPos;

		ImVec2 pos = cursor_pos;
		pos.x += win->Size.x * *value / s_max_timeline_value + TIMELINE_RADIUS;
		pos.y += TIMELINE_RADIUS;

		SetCursorScreenPos(pos - ImVec2(TIMELINE_RADIUS, TIMELINE_RADIUS));
		InvisibleButton(str_id, ImVec2(2 * TIMELINE_RADIUS, 2 * TIMELINE_RADIUS));
		if (IsItemActive() || IsItemHovered())
		{
			ImGui::SetTooltip("%f", *value);
			ImVec2 a(pos.x, GetWindowContentRegionMin().y + win->Pos.y);
			ImVec2 b(pos.x, GetWindowContentRegionMax().y + win->Pos.y);
			win->DrawList->AddLine(a, b, line_color);
		}
		if (IsItemActive() && IsMouseDragging(0))
		{
			*value += GetIO().MouseDelta.x / win->Size.x * s_max_timeline_value;
			changed = true;
		}
		win->DrawList->AddCircleFilled(
			pos, TIMELINE_RADIUS, IsItemActive() || IsItemHovered() ? active_color : inactive_color);
		ImGui::SetCursorScreenPos(cursor_pos);
		return changed;
	}


	void EndTimeline()
	{
		ImGuiWindow* win = GetCurrentWindow();

		ImU32 color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_Button]);
		ImU32 line_color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_Border]);
		ImU32 text_color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_Text]);
		float rounding = GImGui->Style.ScrollbarRounding;
		ImVec2 start(GetWindowContentRegionMin().x + win->Pos.x,
			GetWindowContentRegionMax().y - GetTextLineHeightWithSpacing() + win->Pos.y);
		ImVec2 end = GetWindowContentRegionMax() + win->Pos;

		win->DrawList->AddRectFilled(start, end, color, rounding);

		const int LINE_COUNT = 5;
		const ImVec2 text_offset(0, GetTextLineHeightWithSpacing());
		for (int i = 0; i < LINE_COUNT; ++i)
		{
			ImVec2 a = GetWindowContentRegionMin() + win->Pos + ImVec2(TIMELINE_RADIUS, 0);
			a.x += i * GetWindowContentRegionWidth() / LINE_COUNT;
			ImVec2 b = a;
			b.y = start.y;
			win->DrawList->AddLine(a, b, line_color);
			char tmp[256];
			ImFormatString(tmp, sizeof(tmp), "%.2f", i * s_max_timeline_value / LINE_COUNT);
			win->DrawList->AddText(b, text_color, tmp);
		}

		EndChild();
	}

	void RenderFrameEx(ImVec2 p_min, ImVec2 p_max, bool border, float rounding, float thickness)
	{
		ImGuiWindow* window = GetCurrentWindow();

		if (border)
		{
			window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1),
				GetColorU32(ImGuiCol_BorderShadow), rounding, 15, thickness);
			window->DrawList->AddRect(p_min, p_max, GetColorU32(ImGuiCol_Border), rounding, 15, thickness);
		}
	}

	bool ToolbarButtonEx(ImTextureID texture, const char* tooltip, bool selected, bool enabled)
	{
		return ImageButtonEx(texture, ImVec2(24, 24), tooltip, selected, enabled);
	}

	bool ImageButtonEx(ImTextureID texture, ImVec2 size, const char* tooltip, bool selected, bool enabled)
	{

		ImVec4 bg_color(0, 0, 0, 0);

		auto frame_padding = GetStyle().FramePadding;
		PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
		PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
		PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		PushStyleVar(ImGuiStyleVar_WindowPadding, frame_padding);
		PushStyleVar(ImGuiStyleVar_WindowRounding, 0);

		bool ret = false;

		if (!enabled)
			Image(texture, size, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		else
		{
			if (ImageButton(texture, size, ImVec2(0, 0), ImVec2(1, 1), -1, bg_color))
			{
				ret = true;
			}
		}
		if (tooltip && IsItemHovered())
		{
			SetTooltip("%s", tooltip);
		}

		ImVec2 rectMin = GetItemRectMin();
		ImVec2 rectMax = GetItemRectMax();

		if (selected)
		{
			PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
			RenderFrameEx(rectMin, rectMax, true, 0.0f, 2.0f);
			PopStyleColor();
		}

		PopStyleColor(3);
		PopStyleVar(3);
		return ret;
	}




	static void DrawGradientBar(ImGradient* gradient,
		struct ImVec2 const & bar_pos,
		float maxWidth,
		float height)
	{
		ImVec4 colorA = { 1,1,1,1 };
		ImVec4 colorB = { 1,1,1,1 };
		float prevX = bar_pos.x;
		float barBottom = bar_pos.y + height;
		ImGradientMark* prevMark = nullptr;
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		draw_list->AddRectFilled(ImVec2(bar_pos.x - 2, bar_pos.y - 2),
			ImVec2(bar_pos.x + maxWidth + 2, barBottom + 2),
			IM_COL32(100, 100, 100, 255));

		if (gradient->getMarks().size() == 0)
		{
			draw_list->AddRectFilled(ImVec2(bar_pos.x, bar_pos.y),
				ImVec2(bar_pos.x + maxWidth, barBottom),
				IM_COL32(255, 255, 255, 255));

		}

		ImU32 colorAU32 = 0;
		ImU32 colorBU32 = 0;

		for (auto markIt = gradient->getMarks().begin(); markIt != gradient->getMarks().end(); ++markIt)
		{
			ImGradientMark* mark = *markIt;

			float from = prevX;
			float to = prevX = bar_pos.x + mark->position * maxWidth;

			if (prevMark == nullptr)
			{
				colorA.x = mark->color[0];
				colorA.y = mark->color[1];
				colorA.z = mark->color[2];
			}
			else
			{
				colorA.x = prevMark->color[0];
				colorA.y = prevMark->color[1];
				colorA.z = prevMark->color[2];
			}

			colorB.x = mark->color[0];
			colorB.y = mark->color[1];
			colorB.z = mark->color[2];

			colorAU32 = ImGui::ColorConvertFloat4ToU32(colorA);
			colorBU32 = ImGui::ColorConvertFloat4ToU32(colorB);

			if (mark->position > 0.0)
			{

				draw_list->AddRectFilledMultiColor(ImVec2(from, bar_pos.y),
					ImVec2(to, barBottom),
					colorAU32, colorBU32, colorBU32, colorAU32);
			}

			prevMark = mark;
		}

		if (prevMark && prevMark->position < 1.0)
		{

			draw_list->AddRectFilledMultiColor(ImVec2(prevX, bar_pos.y),
				ImVec2(bar_pos.x + maxWidth, barBottom),
				colorBU32, colorBU32, colorBU32, colorBU32);
		}

		ImGui::SetCursorScreenPos(ImVec2(bar_pos.x, bar_pos.y + height + 10.0f));
	}

	static void DrawGradientMarks(ImGradient* gradient,
		ImGradientMark* & draggingMark,
		ImGradientMark* & selectedMark,
		struct ImVec2 const & bar_pos,
		float maxWidth,
		float height)
	{
		ImVec4 colorA = { 1,1,1,1 };
		ImVec4 colorB = { 1,1,1,1 };
		float barBottom = bar_pos.y + height;
		ImGradientMark* prevMark = nullptr;
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImU32 colorAU32 = 0;
		ImU32 colorBU32 = 0;

		for (auto markIt = gradient->getMarks().begin(); markIt != gradient->getMarks().end(); ++markIt)
		{
			ImGradientMark* mark = *markIt;

			if (!selectedMark)
			{
				selectedMark = mark;
			}

			float to = bar_pos.x + mark->position * maxWidth;

			if (prevMark == nullptr)
			{
				colorA.x = mark->color[0];
				colorA.y = mark->color[1];
				colorA.z = mark->color[2];
			}
			else
			{
				colorA.x = prevMark->color[0];
				colorA.y = prevMark->color[1];
				colorA.z = prevMark->color[2];
			}

			colorB.x = mark->color[0];
			colorB.y = mark->color[1];
			colorB.z = mark->color[2];

			colorAU32 = ImGui::ColorConvertFloat4ToU32(colorA);
			colorBU32 = ImGui::ColorConvertFloat4ToU32(colorB);

			draw_list->AddTriangleFilled(ImVec2(to, bar_pos.y + (height - 6)),
				ImVec2(to - 6, barBottom),
				ImVec2(to + 6, barBottom), IM_COL32(100, 100, 100, 255));

			draw_list->AddRectFilled(ImVec2(to - 6, barBottom),
				ImVec2(to + 6, bar_pos.y + (height + 12)),
				IM_COL32(100, 100, 100, 255), 1.0f, 1.0f);

			draw_list->AddRectFilled(ImVec2(to - 5, bar_pos.y + (height + 1)),
				ImVec2(to + 5, bar_pos.y + (height + 11)),
				IM_COL32(0, 0, 0, 255), 1.0f, 1.0f);

			if (selectedMark == mark)
			{
				draw_list->AddTriangleFilled(ImVec2(to, bar_pos.y + (height - 3)),
					ImVec2(to - 4, barBottom + 1),
					ImVec2(to + 4, barBottom + 1), IM_COL32(0, 255, 0, 255));

				draw_list->AddRect(ImVec2(to - 5, bar_pos.y + (height + 1)),
					ImVec2(to + 5, bar_pos.y + (height + 11)),
					IM_COL32(0, 255, 0, 255), 1.0f, 1.0f);
			}

			draw_list->AddRectFilledMultiColor(ImVec2(to - 3, bar_pos.y + (height + 3)),
				ImVec2(to + 3, bar_pos.y + (height + 9)),
				colorBU32, colorBU32, colorBU32, colorBU32);

			ImGui::SetCursorScreenPos(ImVec2(to - 6, barBottom));
			ImGui::InvisibleButton("mark", ImVec2(12, 12));

			if (ImGui::IsItemHovered())
			{
				if (ImGui::IsMouseClicked(0))
				{
					selectedMark = mark;
					draggingMark = mark;
				}
			}


			prevMark = mark;
		}

		ImGui::SetCursorScreenPos(ImVec2(bar_pos.x, bar_pos.y + height + 20.0f));
	}

	bool GradientButton(ImGradient* gradient)
	{
		if (!gradient) return false;

		ImVec2 widget_pos = ImGui::GetCursorScreenPos();
		// ImDrawList* draw_list = ImGui::GetWindowDrawList();

		float maxWidth = ImMax(250.0f, ImGui::GetContentRegionAvailWidth() - 100.0f);
		bool clicked = ImGui::InvisibleButton("gradient_bar", ImVec2(maxWidth, GRADIENT_BAR_WIDGET_HEIGHT));

		DrawGradientBar(gradient, widget_pos, maxWidth, GRADIENT_BAR_WIDGET_HEIGHT);

		return clicked;
	}

	bool GradientEditor(ImGradient* gradient,
		ImGradientMark* & draggingMark,
		ImGradientMark* & selectedMark)
	{
		if (!gradient) return false;

		bool modified = false;

		ImVec2 bar_pos = ImGui::GetCursorScreenPos();
		bar_pos.x += 10;
		float maxWidth = ImGui::GetContentRegionAvailWidth() - 20;
		float barBottom = bar_pos.y + GRADIENT_BAR_EDITOR_HEIGHT;

		ImGui::InvisibleButton("gradient_editor_bar", ImVec2(maxWidth, GRADIENT_BAR_EDITOR_HEIGHT));

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
		{
			float pos = (ImGui::GetIO().MousePos.x - bar_pos.x) / maxWidth;

			float newMarkCol[4];
			gradient->getColorAt(pos, newMarkCol);


			gradient->addMark(pos, ImColor(newMarkCol[0], newMarkCol[1], newMarkCol[2]));
			modified = true;
		}

		DrawGradientBar(gradient, bar_pos, maxWidth, GRADIENT_BAR_EDITOR_HEIGHT);
		DrawGradientMarks(gradient, draggingMark, selectedMark, bar_pos, maxWidth, GRADIENT_BAR_EDITOR_HEIGHT);

		if (!ImGui::IsMouseDown(0) && draggingMark)
		{
			draggingMark = nullptr;
		}

		if (ImGui::IsMouseDragging(0) && draggingMark)
		{
			float increment = ImGui::GetIO().MouseDelta.x / maxWidth;
			bool insideZone = (ImGui::GetIO().MousePos.x > bar_pos.x) &&
				(ImGui::GetIO().MousePos.x < bar_pos.x + maxWidth);

			if (increment != 0.0f && insideZone)
			{
				draggingMark->position += increment;
				draggingMark->position = ImClamp(draggingMark->position, 0.0f, 1.0f);
				gradient->refreshCache();
				modified = true;
			}

			float diffY = ImGui::GetIO().MousePos.y - barBottom;

			if (diffY >= GRADIENT_MARK_DELETE_DIFFY)
			{
				gradient->removeMark(draggingMark);
				draggingMark = nullptr;
				selectedMark = nullptr;
				modified = true;
			}
		}

		if (!selectedMark && gradient->getMarks().size() > 0)
		{
			selectedMark = gradient->getMarks().front();
		}

		if (selectedMark)
		{
			bool colorModified = ImGui::ColorPicker3("", selectedMark->color, true);

			if (selectedMark && colorModified)
			{
				modified = true;
				gradient->refreshCache();
			}
		}

		return modified;
	}


} // namespace ImGui
