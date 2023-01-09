#pragma once

#include <list>
#include <vector>
#include <string>
#include <cstdint>


struct ImGradientMark
{
	float color[4];
	float position; //0 to 1
};

class ImGradient
{
public:
	ImGradient();
	~ImGradient();

	void getColorAt(float position, float* color) const;
	void addMark(float position, ImColor const color);//
	void removeMark(ImGradientMark* mark);
	void removeAllMark();
	void refreshCache();
	std::list<ImGradientMark*> & getMarks() { return m_marks; }
private:
	void computeColorAt(float position, float* color) const;
	std::list<ImGradientMark*> m_marks;
	float m_cachedValues[256 * 3];
};


namespace ImGui
{

IMGUI_API bool CheckboxEx(const char* label, bool* v);
IMGUI_API ImVec2 GetOsImePosRequest();
IMGUI_API void ResetActiveID();
IMGUI_API int PlotHistogramEx(const char* label,
	float(*values_getter)(void* data, int idx),
	void* data,
	int values_count,
	int values_offset,
	const char* overlay_text,
	float scale_min,
	float scale_max,
	ImVec2 graph_size,
	int selected_index);

IMGUI_API bool ListBox(const char* label,
	int* current_item,
	int scroll_to_item,
	bool(*items_getter)(void*, int, const char**),
	void* data,
	int items_count,
	int height_in_items);
IMGUI_API void BringToFront();
IMGUI_API bool IsFocusedHierarchy();

IMGUI_API bool BeginToolbar(const char* str_id, ImVec2 screen_pos, ImVec2 size);
IMGUI_API void EndToolbar();
IMGUI_API bool ToolbarButton(ImTextureID texture, const ImVec4& bg_color, const char* tooltip);

IMGUI_API void BeginNode(ImGuiID id, ImVec2 screen_pos);
IMGUI_API void EndNode(ImVec2& pos);
IMGUI_API bool NodePin(ImGuiID id, ImVec2 screen_pos);
IMGUI_API void NodeLink(ImVec2 from, ImVec2 to);
IMGUI_API ImVec2 GetNodeInputPos(ImGuiID node_id, int input);
IMGUI_API ImVec2 GetNodeOutputPos(ImGuiID node_id, int output);
IMGUI_API void NodeSlots(int count, bool input);

IMGUI_API bool BeginTimeline(const char* str_id, float max_value);
IMGUI_API bool TimelineEvent(const char* str_id, float* values);
IMGUI_API void EndTimeline();

enum class CurveEditorFlags
{
	NO_TANGENTS = 1 << 0,
	SHOW_GRID = 1 << 1,
	RESET = 1 << 2
};


struct TangentModes
{
	int8_t leftTangent;
	int8_t rightTangent;

	TangentModes()
		: leftTangent(0),
		  rightTangent(0)
	{

	}
};

IMGUI_API std::vector<int> CurveEditor(const char* label
	, std::vector<std::vector<ImVec2>>& values
	, std::vector<int>& points_count
	, std::vector<int>& new_count
	, std::vector<int>& coloridxes
	, std::vector<std::vector<TangentModes>>& tangentsMode
	, const ImVec2& size = ImVec2(-1, -1)
	, const ImVec4& range = ImVec4(-1, -1, -1, -1)
	, ImU32 flags = 0
	, bool edit = true);
IMGUI_API std::pair<int, ImVec2> CurveEditorGetSingleSelect(const char* label);
IMGUI_API void CurveEditorExitSelect(const char* label);
IMGUI_API void CurveEditorFitVertical(const char* label);
IMGUI_API void CurveEditorFitHorizontal(const char* label);
IMGUI_API void CurveEditorTangent(const char* label, const char *tangent);
//IMGUI_API bool BeginResizablePopup(const char* str_id, const ImVec2& size_on_first_use);
IMGUI_API void IntervalGraph(const unsigned long long* value_pairs,
	int value_pairs_count,
	unsigned long long scale_min,
	unsigned long long scele_max);
IMGUI_API bool LabellessInputText(const char* label, char* buf, size_t buf_size, float width = -1);
IMGUI_API void HSplitter(const char* str_id, ImVec2* size);
IMGUI_API void VSplitter(const char* str_id, ImVec2* size);
IMGUI_API void Rect(float w, float h, ImU32 color);
IMGUI_API bool ImageButtonEx(ImTextureID texture, ImVec2 size = ImVec2(24, 24), const char* tooltip = nullptr,
	bool selected = false, bool enabled = true);
IMGUI_API void RenderFrameEx(ImVec2 p_min, ImVec2 p_max, bool border, float rounding, float thickness = 1.0f);
IMGUI_API bool ToolbarButtonEx(ImTextureID texture, const char* tooltip, bool selected, bool enabled);


IMGUI_API	bool GradientButton(ImGradient* gradient);
IMGUI_API	bool GradientEditor(ImGradient* gradient,
	ImGradientMark* & draggingMark,
	ImGradientMark* & selectedMark);

} // namespace ImGui
