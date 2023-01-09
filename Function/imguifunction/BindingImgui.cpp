
#ifdef _EDITOR

#include "BindingImgui.h"

#include <sol/sol.hpp>
#include <imgui.h>
#include <imgui_internal.h>

NS_JYE_BEGIN

class TextureEntity;

#define REGISTER_IMGUI_FUNC(funname)				#funname, &ImGui::funname
#define REGISTER_MEMBER_FUNC(classname, funname)	#funname, &classname::funname
#define REGISTER_MEMBER_FUNC_ImGuiStyle(funname)		REGISTER_MEMBER_FUNC(ImGuiStyle, funname)
#define REGISTER_MEMBER_FUNC_ImGuiViewport(funname)		REGISTER_MEMBER_FUNC(ImGuiViewport, funname)
#define REGISTER_MEMBER_FUNC_ImGuiPayload(funname)		REGISTER_MEMBER_FUNC(ImGuiPayload, funname)

static void Register_ImGui(sol::table& ns_table)
{
	{	// ImVec2
		ns_table.new_usertype<ImVec2>("ImVec2", sol::call_constructor, sol::constructors<
			sol::types<>,
			sol::types<float, float>>(),

			"x", &ImVec2::x,
			"y", &ImVec2::y);
	} 
	{	// ImVec4
		ns_table.new_usertype<ImVec4>("ImVec4", sol::call_constructor, sol::constructors<
			sol::types<>,
			sol::types<float, float, float, float>>(),

			"x", & ImVec4::x,
			"y", & ImVec4::y,
			"z", & ImVec4::z,
			"w", & ImVec4::w);
	}
	{	// ImFont
		ns_table.new_usertype<ImFont>("ImFont", sol::call_constructor, sol::constructors<
			sol::types<>>()
			);
	}

	sol::table imgui_table = ns_table["ImGui"].get_or_create<sol::table>();

	imgui_table.set_function("Begin", sol::overload(
		[](const char* name, bool p_open) { 
			bool b_open = p_open;
			return std::make_tuple(ImGui::Begin(name, &b_open, 0), b_open);
		},
		[](const char* name, bool p_open, ImGuiWindowFlags flags) {
			bool b_open = p_open;
			return std::make_tuple(ImGui::Begin(name, &b_open, flags), b_open);
		}
		));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(End));

	imgui_table.set_function("BeginChild", sol::overload(
		[](const char* str_id) { return ImGui::BeginChild(str_id, ImVec2(0, 0), false, 0); },
		[](const char* str_id, const ImVec2& size) { return ImGui::BeginChild(str_id, size, false, 0); },
		[](const char* str_id, const ImVec2& size, bool border) { return ImGui::BeginChild(str_id, size, border, 0); },
		[](const char* str_id, const ImVec2& size, bool border, ImGuiWindowFlags flags) { return ImGui::BeginChild(str_id, size, border, flags); },
		[](ImGuiID id) { ImGui::BeginChild(id, ImVec2(0, 0), false, 0); },
		[](ImGuiID id, const ImVec2& size) { ImGui::BeginChild(id, size, false, 0); },
		[](ImGuiID id, const ImVec2& size, bool border) { ImGui::BeginChild(id, size, border, 0); },
		[](ImGuiID id, const ImVec2& size, bool border, ImGuiWindowFlags flags) { ImGui::BeginChild(id, size, border, flags); }
	));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(EndChild));

	imgui_table.set_function(REGISTER_IMGUI_FUNC(IsWindowAppearing));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(IsWindowCollapsed));
	imgui_table.set_function("IsWindowFocused", sol::overload(
		[]() { return ImGui::IsWindowFocused(0); },
		[](ImGuiFocusedFlags flags) { return ImGui::IsWindowFocused(flags); }
	));
	imgui_table.set_function("IsWindowHovered", sol::overload(
		[]() { return ImGui::IsWindowHovered(0); },
		[](ImGuiHoveredFlags flags) { return ImGui::IsWindowHovered(flags); }
	));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetWindowDrawList));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetWindowDpiScale));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetWindowPos));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetWindowSize));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetWindowWidth));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetWindowHeight));
	//imgui_table.set_function(REGISTER_IMGUI_FUNC(GetWindowViewport));

	// Window manipulation
	imgui_table.set_function("SetNextWindowPos", sol::overload(
		[](const ImVec2& size) { ImGui::SetNextWindowPos(size, 0, ImVec2(0, 0)); },
		[](const ImVec2& size, ImGuiCond cond) { ImGui::SetNextWindowPos(size, cond, ImVec2(0, 0)); },
		[](const ImVec2& size, ImGuiCond cond, const ImVec2& pivot) { ImGui::SetNextWindowPos(size, cond, pivot); }
	));
	imgui_table.set_function("SetNextWindowSize", sol::overload(
		[](const ImVec2& size) { ImGui::SetNextWindowSize(size, 0); },
		[](const ImVec2& size, ImGuiCond cond) { ImGui::SetNextWindowSize(size, cond); }
	));
	imgui_table.set_function("SetNextWindowSizeConstraints", sol::overload(
		[](const ImVec2& size_min, const ImVec2& size_max) { ImGui::SetNextWindowSizeConstraints(size_min, size_max); }
	));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(SetNextWindowContentSize));
	imgui_table.set_function("SetNextWindowCollapsed", sol::overload(
		[](bool collapsed) { ImGui::SetNextWindowCollapsed(collapsed, 0); },
		[](bool collapsed, ImGuiCond cond) { ImGui::SetNextWindowCollapsed(collapsed, cond); }
	));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(SetNextWindowFocus));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(SetNextWindowBgAlpha));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(SetNextWindowViewport));
	imgui_table.set_function("SetWindowPos", sol::overload(
		[](const ImVec2& size) { ImGui::SetWindowPos(size, 0); },
		[](const ImVec2& size, ImGuiCond cond) { ImGui::SetWindowPos(size, cond); },
		[](const char* name, const ImVec2& size) { ImGui::SetWindowPos(name, size, 0); },
		[](const char* name, const ImVec2& size, ImGuiCond cond) { ImGui::SetWindowPos(name, size, cond); }
	));
	imgui_table.set_function("SetWindowSize", sol::overload(
		[](const ImVec2& size) { ImGui::SetWindowSize(size, 0); },
		[](const ImVec2& size, ImGuiCond cond) { ImGui::SetWindowSize(size, cond); },
		[](const char* name, const ImVec2& size) { ImGui::SetWindowSize(name, size, 0); },
		[](const char* name, const ImVec2& size, ImGuiCond cond) { ImGui::SetWindowSize(name, size, cond); }
	));
	imgui_table.set_function("SetWindowCollapsed", sol::overload(
		[](bool collapsed) { ImGui::SetWindowCollapsed(collapsed, 0); },
		[](bool collapsed, ImGuiCond cond) { ImGui::SetWindowCollapsed(collapsed, cond); },
		[](const char* name, bool collapsed) { ImGui::SetWindowCollapsed(name, collapsed, 0); },
		[](const char* name, bool collapsed, ImGuiCond cond) { ImGui::SetWindowCollapsed(name, collapsed, cond); }
	));
	imgui_table.set_function("SetWindowFocus", sol::overload(
		(void (*)()) & ImGui::SetWindowFocus,
		(void (*)(const char*)) & ImGui::SetWindowFocus
	));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(SetWindowFontScale));

	// Content region
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetContentRegionAvail));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetContentRegionMax));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetWindowContentRegionMin));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetWindowContentRegionMax));

	// Windows Scrolling
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetScrollX));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetScrollY));
	imgui_table.set_function("SetScrollX", sol::overload(
		[](float scroll_x) { ImGui::SetScrollX(scroll_x); }
		));
	imgui_table.set_function("SetScrollY", sol::overload(
		[](float scroll_y) { ImGui::SetScrollY(scroll_y); }
		));
	//imgui_table.set_function(REGISTER_IMGUI_FUNC(SetScrollX));
	//imgui_table.set_function(REGISTER_IMGUI_FUNC(SetScrollY));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetScrollMaxX));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetScrollMaxY));
	/*imgui_table.set_function("SetScrollHereX", sol::overload(
		[]() { ImGui::SetScrollHereX(0.5f); },
		[](float center_x_ratio) { ImGui::SetScrollHereX(center_x_ratio); }
	));*/
	imgui_table.set_function("SetScrollHereY", sol::overload(
		[]() { ImGui::SetScrollHereY(0.5f); },
		[](float center_x_ratio) { ImGui::SetScrollHereY(center_x_ratio); }
	));
	/*imgui_table.set_function("SetScrollFromPosX", sol::overload(
		[](float local_x) { ImGui::SetScrollFromPosX(local_x, 0.5f); },
		[](float local_x, float center_x_ratio) { ImGui::SetScrollFromPosX(local_x, center_x_ratio); }
	)); */
	imgui_table.set_function("SetScrollFromPosY", sol::overload(
		[](float local_x) { ImGui::SetScrollFromPosY(local_x, 0.5f); },
		[](float local_x, float center_x_ratio) { ImGui::SetScrollFromPosY(local_x, center_x_ratio); }
	));

	// Parameters stacks (shared)
	imgui_table.set_function(REGISTER_IMGUI_FUNC(PushFont));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(PopFont));
	imgui_table.set_function("PushStyleColor", sol::overload(
		[](ImGuiCol idx, ImU32 col) { ImGui::PushStyleColor(idx, col); },
		[](ImGuiCol idx, const ImVec4& col) { ImGui::PushStyleColor(idx, col); }
	));
	imgui_table.set_function("PopStyleColor", sol::overload(
		[]() { ImGui::PopStyleColor(1); },
		[](int count) { ImGui::PopStyleColor(count); }
	));
	imgui_table.set_function("PushStyleVar", sol::overload(
		[](ImGuiCol idx, float val) { ImGui::PushStyleVar(idx, val); },
		[](ImGuiCol idx, const ImVec2& val) { ImGui::PushStyleVar(idx, val); }
	));
	imgui_table.set_function("PopStyleVar", sol::overload(
		[]() { ImGui::PopStyleVar(1); },
		[](int count) { ImGui::PopStyleVar(count); }
	));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(PushAllowKeyboardFocus));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(PopAllowKeyboardFocus));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(PushButtonRepeat));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(PopButtonRepeat));

	// Parameters stacks (current window)
	imgui_table.set_function(REGISTER_IMGUI_FUNC(PushItemWidth));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(PopItemWidth));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(SetNextItemWidth));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(CalcItemWidth));
	imgui_table.set_function("PushTextWrapPos", sol::overload(
		[]() { ImGui::PushTextWrapPos(0.0f); },
		[](float wrap_local_pos_x) { ImGui::PushTextWrapPos(wrap_local_pos_x); }
	));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(PopTextWrapPos));

	// Style read access
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetFont));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetFontSize));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetFontTexUvWhitePixel));
	imgui_table.set_function("GetColorU32", sol::overload(
		[](ImGuiCol idx) { return ImGui::GetColorU32(idx, 1.0f); },
		[](ImGuiCol idx, float alpha_mul) { return ImGui::GetColorU32(idx, alpha_mul); },
		[](const ImVec4& val) { return ImGui::GetColorU32(val); },
		[](ImU32 col) { return ImGui::GetColorU32(col); }
	));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetStyleColorVec4));

	// Cursor / Layout
	imgui_table.set_function(REGISTER_IMGUI_FUNC(Separator));
	imgui_table.set_function("SameLine", sol::overload(
		[]() { ImGui::SameLine(0.0f, -1.0f); },
		[](float offset_from_start_x) { ImGui::SameLine(offset_from_start_x, -1.0f); },
		[](float offset_from_start_x, float spacing) { ImGui::SameLine(offset_from_start_x, spacing); }
	));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(NewLine));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(Spacing));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(Dummy));
	imgui_table.set_function("Indent", sol::overload(
		[]() { ImGui::Indent(0.0f); },
		[](float indent_w) { ImGui::Indent(indent_w); }
	));
	imgui_table.set_function("Unindent", sol::overload(
		[]() { ImGui::Unindent(0.0f); },
		[](float indent_w) { ImGui::Unindent(indent_w); }
	));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(BeginGroup));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(EndGroup));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetCursorPos));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetCursorPosX));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetCursorPosY));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(SetCursorPos));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(SetCursorPosX));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(SetCursorPosY));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetCursorStartPos));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetCursorScreenPos));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(SetCursorScreenPos));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(AlignTextToFramePadding));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetTextLineHeight));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetTextLineHeightWithSpacing));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetFrameHeight));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetFrameHeightWithSpacing));

	// ID stack/scopes
	imgui_table.set_function("PushID", sol::overload(
		[](const char* str_id) { ImGui::PushID(str_id); },
		[](const char* str_id, const char* str_id_end) { ImGui::PushID(str_id, str_id_end); },
		[](int id) { ImGui::PushID(id); }
	));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(PopID));
	imgui_table.set_function("GetID", sol::overload(
		[](const char* str_id) { return ImGui::GetID(str_id); },
		[](const char* str_id, const char* str_id_end) { return ImGui::GetID(str_id, str_id_end); }
	));

	// Widgets: Text
	imgui_table.set_function("TextUnformatted", sol::overload(
		[](const char* text) { ImGui::TextUnformatted(text, NULL); },
		[](const char* text, const char* text_end) { ImGui::TextUnformatted(text, text_end); }
	));
	imgui_table.set_function("Text", sol::overload(
		[](const char* text) { ImGui::Text(text); }
	));
	imgui_table.set_function("TextColored", sol::overload(
		[](const ImVec4& col, const char* text) { ImGui::TextColored(col, text); }
	));
	imgui_table.set_function("TextDisabled", sol::overload(
		[](const char* text) { ImGui::TextDisabled(text); }
	));
	imgui_table.set_function("TextWrapped", sol::overload(
		[](const char* text) { ImGui::TextWrapped(text); }
	));
	imgui_table.set_function("LabelText", sol::overload(
		[](const char* label, const char* text) { ImGui::LabelText(label, text); }
	));
	imgui_table.set_function("BulletText", sol::overload(
		[](const char* text) { ImGui::BulletText(text); }
	));

	// Widgets: Main
	imgui_table.set_function("Button", sol::overload(
		[](const char* label)->bool { return ImGui::Button(label, ImVec2(0, 0)); },
		[](const char* label, const ImVec2& size)->bool { return ImGui::Button(label, size); }
	));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(SmallButton));
	imgui_table.set_function("InvisibleButton", sol::overload(
		[](const char* str_id, const ImVec2& size)->bool { return ImGui::InvisibleButton(str_id, size); }/*,
		[](const char* str_id, const ImVec2& size, ImGuiButtonFlags flags)->bool { return ImGui::InvisibleButton(str_id, size, flags); }*/
	));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(ArrowButton));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(Checkbox));
	/*imgui_table.set_function("CheckboxFlags", sol::overload(
		[](const char* label, int* flags, int flags_value)->bool { return ImGui::CheckboxFlags(label, flags, flags_value); },
		[](const char* label, unsigned int* flags, unsigned int flags_value)->bool { return ImGui::CheckboxFlags(label, flags, flags_value); }
	));*/
	imgui_table.set_function("RadioButton", sol::overload(
		[](const char* label, bool active) { 
			return ImGui::RadioButton(label, active); 
		},
		[](const char* label, int v, int v_button) { 
			int vres = v;
			bool bres = ImGui::RadioButton(label, &vres, v_button);
			return std::make_tuple(bres, vres);
		}
	));
	//imgui_table.set_function(REGISTER_IMGUI_FUNC(ProgressBar));
	//imgui_table.set_function(REGISTER_IMGUI_FUNC(Bullet));

	// Widgets: Images
	imgui_table.set_function("Image", sol::overload(
		[](TextureEntity* user_texture_id, 
			const ImVec2& size) { 
				return ImGui::Image(user_texture_id, size, ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));
		},
		[](TextureEntity* user_texture_id, 
			const ImVec2& size, 
			const ImVec2& uv0) { 
				return ImGui::Image(user_texture_id, size, uv0, ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));
		},
		[](TextureEntity* user_texture_id, 
			const ImVec2& size, 
			const ImVec2& uv0, 
			const ImVec2& uv1) { 
				return ImGui::Image(user_texture_id, size, uv0, uv1, ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));
		},
		[](TextureEntity* user_texture_id, 
			const ImVec2& size, 
			const ImVec2& uv0, 
			const ImVec2& uv1, 
			const ImVec4& tint_col) { 
				return ImGui::Image(user_texture_id, size, uv0, uv1, tint_col, ImVec4(0, 0, 0, 0));
		},
		[](TextureEntity* user_texture_id, 
			const ImVec2& size, 
			const ImVec2& uv0, 
			const ImVec2& uv1, 
			const ImVec4& tint_col, 
			const ImVec4& border_col) { 
				return ImGui::Image(user_texture_id, size, uv0, uv1, tint_col, border_col);
		}
	));
	/*imgui_table.set_function("ImageButton", sol::overload(
		[](const char* str_id, 
			TextureEntity* user_texture_id,
			const ImVec2& size)->bool {
				return ImGui::ImageButton(str_id, user_texture_id, size, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));
		},
		[](const char* str_id, 
			TextureEntity* user_texture_id,
			const ImVec2& size, 
			const ImVec2& uv0)->bool {
				return ImGui::ImageButton(str_id, user_texture_id, size, uv0, ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));
		},
		[](const char* str_id, 
			TextureEntity* user_texture_id,
			const ImVec2& size, 
			const ImVec2& uv0, 
			const ImVec2& uv1)->bool {
				return ImGui::ImageButton(str_id, user_texture_id, size, uv0, uv1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));
		},
		[](const char* str_id, 
			TextureEntity* user_texture_id,
			const ImVec2& size, 
			const ImVec2& uv0, 
			const ImVec2& uv1, 
			const ImVec4& bg_col)->bool {
				return ImGui::ImageButton(str_id, user_texture_id, size, uv0, uv1, bg_col, ImVec4(1, 1, 1, 1));
		},
		[](const char* str_id, 
			TextureEntity* user_texture_id,
			const ImVec2& size, 
			const ImVec2& uv0,
			const ImVec2& uv1,
			const ImVec4& bg_col, 
			const ImVec4& tint_col)->bool {
				return ImGui::ImageButton(str_id, user_texture_id, size, uv0, uv1, bg_col, tint_col);
		}
	));*/

	// Widgets: Combo Box
	imgui_table.set_function("BeginCombo", sol::overload(
		[](const char* label, const char* preview_value)->bool {
			return ImGui::BeginCombo(label, preview_value, 0);
		},
		[](const char* label, const char* preview_value, ImGuiComboFlags flags)->bool {
			return ImGui::BeginCombo(label, preview_value, flags);
		}
	));
	imgui_table.set_function(REGISTER_IMGUI_FUNC(EndCombo));
	imgui_table.set_function("Combo", sol::overload(
		[](const char* label, int* current_item, const char* const items[], int items_count)->bool {
			return ImGui::Combo(label, current_item, items, items_count, -1);
		},
		[](const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items)->bool {
			return ImGui::Combo(label, current_item, items, items_count, popup_max_height_in_items);
		},
		[](const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items)->bool {
			return ImGui::Combo(label, current_item, items_separated_by_zeros, popup_max_height_in_items);
		},
		[](const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items)->bool {
			return ImGui::Combo(label, current_item, items_separated_by_zeros, popup_max_height_in_items);
		}
		// ImGui::Combo
	));

	{	// Widgets: Drag Sliders
		imgui_table.set_function("DragFloat", sol::overload(
			[](const char* label, float* v) ->bool {
				return ImGui::DragFloat(label, v, 1.0f, 0.0f, 0.0f, "%.3f", 0);
			},
			[](const char* label, float* v, float v_speed) ->bool {
				return ImGui::DragFloat(label, v, v_speed, 0.0f, 0.0f, "%.3f", 0);
			},
			[](const char* label, float* v, float v_speed, float v_min, float v_max) ->bool {
				return ImGui::DragFloat(label, v, v_speed, v_min, v_max, "%.3f", 0);
			},
			[](const char* label, float* v, float v_speed, float v_min, float v_max, const char* format) ->bool {
				return ImGui::DragFloat(label, v, v_speed, v_min, v_max, format, 0);
			}/*,
			[](const char* label, float* v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::DragFloat(label, v, v_speed, v_min, v_max, format, flags);
			}*/
		));
		imgui_table.set_function("DragFloat2", sol::overload(
			[](const char* label, float v[2]) ->bool {
				return ImGui::DragFloat2(label, v, 1.0f, 0.0f, 0.0f, "%.3f", 0);
			},
			[](const char* label, float v[2], float v_speed) ->bool {
				return ImGui::DragFloat2(label, v, v_speed, 0.0f, 0.0f, "%.3f", 0);
			},
			[](const char* label, float v[2], float v_speed, float v_min, float v_max) ->bool {
				return ImGui::DragFloat2(label, v, v_speed, v_min, v_max, "%.3f", 0);
			},
			[](const char* label, float v[2], float v_speed, float v_min, float v_max, const char* format) ->bool {
				return ImGui::DragFloat2(label, v, v_speed, v_min, v_max, format, 0);
			}/*,
			[](const char* label, float v[2], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::DragFloat2(label, v, v_speed, v_min, v_max, format, flags);
			}*/
		));
		imgui_table.set_function("DragFloat3", sol::overload(
			[](const char* label, float v[3]) ->bool {
				return ImGui::DragFloat3(label, v, 1.0f, 0.0f, 0.0f, "%.3f", 0);
			},
			[](const char* label, float v[3], float v_speed) ->bool {
				return ImGui::DragFloat3(label, v, v_speed, 0.0f, 0.0f, "%.3f", 0);
			},
			[](const char* label, float v[3], float v_speed, float v_min, float v_max) ->bool {
				return ImGui::DragFloat3(label, v, v_speed, v_min, v_max, "%.3f", 0);
			},
			[](const char* label, float v[3], float v_speed, float v_min, float v_max, const char* format) ->bool {
				return ImGui::DragFloat3(label, v, v_speed, v_min, v_max, format, 0);
			}/*,
			[](const char* label, float v[3], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::DragFloat3(label, v, v_speed, v_min, v_max, format, flags);
			}*/
		));
		imgui_table.set_function("DragFloat4", sol::overload(
			[](const char* label, float v[4]) ->bool {
				return ImGui::DragFloat4(label, v, 1.0f, 0.0f, 0.0f, "%.3f", 0);
			},
			[](const char* label, float v[4], float v_speed) ->bool {
				return ImGui::DragFloat4(label, v, v_speed, 0.0f, 0.0f, "%.3f", 0);
			},
			[](const char* label, float v[4], float v_speed, float v_min, float v_max) ->bool {
				return ImGui::DragFloat4(label, v, v_speed, v_min, v_max, "%.3f", 0);
			},
			[](const char* label, float v[4], float v_speed, float v_min, float v_max, const char* format) ->bool {
				return ImGui::DragFloat4(label, v, v_speed, v_min, v_max, format, 0);
			}/*,
			[](const char* label, float v[4], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::DragFloat4(label, v, v_speed, v_min, v_max, format, flags);
			}*/
		));
		imgui_table.set_function("DragFloatRange2", sol::overload(
			[](const char* label, float* v_current_min, float* v_current_max, float v_speed, float v_min, float v_max, const char* format) ->bool {
				return ImGui::DragFloatRange2(label, v_current_min, v_current_max, v_speed, v_min, v_max, format, NULL, 0);
			},
			[](const char* label, float* v_current_min, float* v_current_max, float v_speed, float v_min, float v_max, const char* format, const char* format_max) ->bool {
				return ImGui::DragFloatRange2(label, v_current_min, v_current_max, v_speed, v_min, v_max, format, format_max, 0);
			}/*,
			[](const char* label, float* v_current_min, float* v_current_max, float v_speed, float v_min, float v_max, const char* format, const char* format_max, ImGuiSliderFlags flags) ->bool {
				return ImGui::DragFloatRange2(label, v_current_min, v_current_max, v_speed, v_min, v_max, format, format_max, flags);
			}*/
		));
		imgui_table.set_function("DragInt", sol::overload(
			[](const char* label, int* v) ->bool {
				return ImGui::DragInt(label, v, 1.0f, 0.0f, 0.0f, "%.3f");
			},
			[](const char* label, int* v, float v_speed) ->bool {
				return ImGui::DragInt(label, v, v_speed, 0.0f, 0.0f, "%.3f");
			},
			[](const char* label, int* v, float v_speed, int v_min, int v_max) ->bool {
				return ImGui::DragInt(label, v, v_speed, v_min, v_max, "%.3f");
			},
			[](const char* label, int* v, float v_speed, int v_min, int v_max, const char* format) ->bool {
				return ImGui::DragInt(label, v, v_speed, v_min, v_max, format);
			}/*,
			[](const char* label, int* v, float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::DragInt(label, v, v_speed, v_min, v_max, format, flags);
			}*/
		));
		imgui_table.set_function("DragInt2", sol::overload(
			[](const char* label, int v[2]) ->bool {
				return ImGui::DragInt2(label, v, 1.0f, 0.0f, 0.0f, "%.3f");
			},
			[](const char* label, int v[2], float v_speed) ->bool {
				return ImGui::DragInt2(label, v, v_speed, 0.0f, 0.0f, "%.3f");
			},
			[](const char* label, int v[2], float v_speed, int v_min, int v_max) ->bool {
				return ImGui::DragInt2(label, v, v_speed, v_min, v_max, "%.3f");
			},
			[](const char* label, int v[2], float v_speed, int v_min, int v_max, const char* format) ->bool {
				return ImGui::DragInt2(label, v, v_speed, v_min, v_max, format);
			}/*,
			[](const char* label, int v[2], float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::DragInt2(label, v, v_speed, v_min, v_max, format, flags);
			}*/
		));
		imgui_table.set_function("DragInt3", sol::overload(
			[](const char* label, int v[3]) ->bool {
				return ImGui::DragInt3(label, v, 1.0f, 0, 0, "%.3f");
			},
			[](const char* label, int v[3], float v_speed) ->bool {
				return ImGui::DragInt3(label, v, v_speed, 0, 0, "%.3f");
			},
			[](const char* label, int v[3], float v_speed, int v_min, int v_max) ->bool {
				return ImGui::DragInt3(label, v, v_speed, v_min, v_max, "%.3f");
			},
			[](const char* label, int v[3], float v_speed, int v_min, int v_max, const char* format) ->bool {
				return ImGui::DragInt3(label, v, v_speed, v_min, v_max, format);
			}/*,
			[](const char* label, int v[3], float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::DragInt3(label, v, v_speed, v_min, v_max, format, flags);
			}*/
		));
		imgui_table.set_function("DragInt4", sol::overload(
			[](const char* label, int v[4]) ->bool {
				return ImGui::DragInt4(label, v, 1.0f, 0.0f, 0.0f, "%.3f");
			},
			[](const char* label, int v[4], float v_speed) ->bool {
				return ImGui::DragInt4(label, v, v_speed, 0.0f, 0.0f, "%.3f");
			},
			[](const char* label, int v[4], float v_speed, int v_min, int v_max) ->bool {
				return ImGui::DragInt4(label, v, v_speed, v_min, v_max, "%.3f");
			},
			[](const char* label, int v[4], float v_speed, int v_min, int v_max, const char* format) ->bool {
				return ImGui::DragInt4(label, v, v_speed, v_min, v_max, format);
			}/*,
			[](const char* label, int v[4], float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::DragInt4(label, v, v_speed, v_min, v_max, format, flags);
			}*/
		));
		imgui_table.set_function("DragIntRange2", sol::overload(
			[](const char* label, int* v_current_min, int* v_current_max, float v_speed, int v_min, int v_max, const char* format) ->bool {
				return ImGui::DragIntRange2(label, v_current_min, v_current_max, v_speed, v_min, v_max, format, format);
			}/*,
			[](const char* label, int* v_current_min, int* v_current_max, float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::DragIntRange2(label, v_current_min, v_current_max, v_speed, v_min, v_max, format, format, flags);
			}*/
		));
		imgui_table.set_function("DragScalar", sol::overload(
			[](const char* label, ImGuiDataType data_type, void* p_data) ->bool {
				return ImGui::DragScalar(label, data_type, p_data, 1.0f, NULL, NULL, NULL, 0);
			},
			[](const char* label, ImGuiDataType data_type, void* p_data, float v_speed) ->bool {
				return ImGui::DragScalar(label, data_type, p_data, v_speed, NULL, NULL, NULL, 0);
			},
			[](const char* label, ImGuiDataType data_type, void* p_data, float v_speed, const void* p_min, const void* p_max) ->bool {
				return ImGui::DragScalar(label, data_type, p_data, v_speed, p_min, p_max, NULL, 0);
			},
			[](const char* label, ImGuiDataType data_type, void* p_data, float v_speed, const void* p_min, const void* p_max, const char* format) ->bool {
				return ImGui::DragScalar(label, data_type, p_data, v_speed, p_min, p_max, format, 0);
			}/*,
			[](const char* label, ImGuiDataType data_type, void* p_data, float v_speed, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::DragScalar(label, data_type, p_data, v_speed, p_min, p_max, format, flags);
			}*/
		));
		imgui_table.set_function("DragScalarN", sol::overload(
			[](const char* label, ImGuiDataType data_type, void* p_data, int components) ->bool {
				return ImGui::DragScalarN(label, data_type, p_data, components, 1.0f, NULL, NULL, NULL, 0);
			},
			[](const char* label, ImGuiDataType data_type, void* p_data, int components, float v_speed) ->bool {
				return ImGui::DragScalarN(label, data_type, p_data, components, v_speed, NULL, NULL, NULL, 0);
			},
			[](const char* label, ImGuiDataType data_type, void* p_data, int components, float v_speed, const void* p_min, const void* p_max) ->bool {
				return ImGui::DragScalarN(label, data_type, p_data, components, v_speed, p_min, p_max, NULL, 0);
			},
			[](const char* label, ImGuiDataType data_type, void* p_data, int components, float v_speed, const void* p_min, const void* p_max, const char* format) ->bool {
				return ImGui::DragScalarN(label, data_type, p_data, components, v_speed, p_min, p_max, format, 0);
			}/*,
			[](const char* label, ImGuiDataType data_type, void* p_data, int components, float v_speed, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::DragScalarN(label, data_type, p_data, components, v_speed, p_min, p_max, format, flags);
			}*/
		));

	}
	
	{	// Widgets: Regular Sliders
		imgui_table.set_function("SliderFloat", sol::overload(
			[](const char* label, float* v, float v_min, float v_max) ->bool {
				return ImGui::SliderFloat(label, v, v_min, v_max, "%.3f", 0);
			},
			[](const char* label, float* v, float v_min, float v_max, const char* format) ->bool {
				return ImGui::SliderFloat(label, v, v_min, v_max, format, 0);
			},
			[](const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::SliderFloat(label, v, v_min, v_max, format, flags);
			}
		));
		imgui_table.set_function("SliderFloat2", sol::overload(
			[](const char* label, float v[2], float v_min, float v_max) ->bool {
				return ImGui::SliderFloat2(label, v, v_min, v_max, "%.3f", 0);
			},
			[](const char* label, float v[2], float v_min, float v_max, const char* format) ->bool {
				return ImGui::SliderFloat2(label, v, v_min, v_max, format, 0);
			},
			[](const char* label, float v[2], float v_min, float v_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::SliderFloat2(label, v, v_min, v_max, format, flags);
			}
		));
		imgui_table.set_function("SliderFloat3", sol::overload(
			[](const char* label, float v[3], float v_min, float v_max) ->bool {
				return ImGui::SliderFloat3(label, v, v_min, v_max, "%.3f", 0);
			},
			[](const char* label, float v[3], float v_min, float v_max, const char* format) ->bool {
				return ImGui::SliderFloat3(label, v, v_min, v_max, format, 0);
			},
			[](const char* label, float v[3], float v_min, float v_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::SliderFloat3(label, v, v_min, v_max, format, flags);
			}
		));
		imgui_table.set_function("SliderFloat4", sol::overload(
			[](const char* label, float v[4], float v_min, float v_max) ->bool {
				return ImGui::SliderFloat4(label, v, v_min, v_max, "%.3f", 0);
			},
			[](const char* label, float v[4], float v_min, float v_max, const char* format) ->bool {
				return ImGui::SliderFloat4(label, v, v_min, v_max, format, 0);
			},
			[](const char* label, float v[4], float v_min, float v_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::SliderFloat4(label, v, v_min, v_max, format, flags);
			}
		));
		imgui_table.set_function("SliderAngle", sol::overload(
			[](const char* label, float* v_rad) ->bool {
				return ImGui::SliderAngle(label, v_rad, -360.0f, +360.0f, "%.0f deg");
			},
			[](const char* label, float* v_rad, float v_degrees_min, float v_degrees_max) ->bool {
				return ImGui::SliderAngle(label, v_rad, v_degrees_min, v_degrees_max, "%.0f deg");
			},
			[](const char* label, float* v_rad, float v_degrees_min, float v_degrees_max, const char* format) ->bool {
				return ImGui::SliderAngle(label, v_rad, v_degrees_min, v_degrees_max, format);
			}/*,
			[](const char* label, float* v_rad, float v_degrees_min, float v_degrees_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::SliderAngle(label, v_rad, v_degrees_min, v_degrees_max, format, flags);
			}*/
		));
		imgui_table.set_function("SliderInt", sol::overload(
			[](const char* label, int* v, int v_min, int v_max) ->bool {
				return ImGui::SliderInt(label, v, v_min, v_max, "%.3f");
			},
			[](const char* label, int* v, int v_min, int v_max, const char* format) ->bool {
				return ImGui::SliderInt(label, v, v_min, v_max, format);
			}/*,
			[](const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::SliderInt(label, v, v_min, v_max, format, flags);
			}*/
		));
		imgui_table.set_function("SliderInt2", sol::overload(
			[](const char* label, int v[2], int v_min, int v_max) ->bool {
				return ImGui::SliderInt2(label, v, v_min, v_max, "%.3f");
			},
			[](const char* label, int v[2], int v_min, int v_max, const char* format) ->bool {
				return ImGui::SliderInt2(label, v, v_min, v_max, format);
			}/*,
			[](const char* label, int v[2], int v_min, int v_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::SliderInt2(label, v, v_min, v_max, format, flags);
			}*/
		));
		imgui_table.set_function("SliderInt3", sol::overload(
			[](const char* label, int v[3], int v_min, int v_max) ->bool {
				return ImGui::SliderInt3(label, v, v_min, v_max, "%.3f");
			},
			[](const char* label, int v[3], int v_min, int v_max, const char* format) ->bool {
				return ImGui::SliderInt3(label, v, v_min, v_max, format);
			}/*,
			[](const char* label, int v[3], int v_min, int v_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::SliderInt3(label, v, v_min, v_max, format, flags);
			}*/
		));
		imgui_table.set_function("SliderInt4", sol::overload(
			[](const char* label, int v[4], int v_min, int v_max) ->bool {
				return ImGui::SliderInt4(label, v, v_min, v_max, "%.3f");
			},
			[](const char* label, int v[4], int v_min, int v_max, const char* format) ->bool {
				return ImGui::SliderInt4(label, v, v_min, v_max, format);
			}/*,
			[](const char* label, int v[4], int v_min, int v_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::SliderInt4(label, v, v_min, v_max, format, flags);
			}*/
		));
		imgui_table.set_function("SliderScalar", sol::overload(
			[](const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max) ->bool {
				return ImGui::SliderScalar(label, data_type, p_data, p_min, p_max, NULL);
			},
			[](const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format) ->bool {
				return ImGui::SliderScalar(label, data_type, p_data, p_min, p_max, format);
			}/*,
			[](const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::SliderScalar(label, data_type, p_data, p_min, p_max, format, flags);
			}*/
		));
		imgui_table.set_function("SliderScalarN", sol::overload(
			[](const char* label, ImGuiDataType data_type, void* p_data, int components, const void* p_min, const void* p_max) ->bool {
				return ImGui::SliderScalarN(label, data_type, p_data, components, p_min, p_max, NULL, 0);
			},
			[](const char* label, ImGuiDataType data_type, void* p_data, int components, const void* p_min, const void* p_max, const char* format) ->bool {
				return ImGui::SliderScalarN(label, data_type, p_data, components, p_min, p_max, format, 0);
			}/*,
			[](const char* label, ImGuiDataType data_type, void* p_data, int components, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::SliderScalarN(label, data_type, p_data, components, p_min, p_max, format, flags);
			}*/
		));
		imgui_table.set_function("VSliderFloat", sol::overload(
			[](const char* label, const ImVec2& size, float* v, float v_min, float v_max) ->bool {
				return ImGui::VSliderFloat(label, size, v, v_min, v_max, "%.3f", 0);
			},
			[](const char* label, const ImVec2& size, float* v, float v_min, float v_max, const char* format) ->bool {
				return ImGui::VSliderFloat(label, size, v, v_min, v_max, format, 0);
			},
			[](const char* label, const ImVec2& size, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::VSliderFloat(label, size, v, v_min, v_max, format, flags);
			}
		));
		imgui_table.set_function("VSliderInt", sol::overload(
			[](const char* label, const ImVec2& size, int* v, int v_min, int v_max) ->bool {
				return ImGui::VSliderInt(label, size, v, v_min, v_max, "%.3f");
			},
			[](const char* label, const ImVec2& size, int* v, int v_min, int v_max, const char* format) ->bool {
				return ImGui::VSliderInt(label, size, v, v_min, v_max, format);
			}/*,
			[](const char* label, const ImVec2& size, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::VSliderInt(label, size, v, v_min, v_max, format, flags);
			}*/
		));
		imgui_table.set_function("VSliderScalar", sol::overload(
			[](const char* label, const ImVec2& size, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max) ->bool {
				return ImGui::VSliderScalar(label, size, data_type, p_data, p_min, p_max, "%.3f", 0);
			},
			[](const char* label, const ImVec2& size, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format) ->bool {
				return ImGui::VSliderScalar(label, size, data_type, p_data, p_min, p_max, format, 0);
			}/*,
			[](const char* label, const ImVec2& size, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags) ->bool {
				return ImGui::VSliderScalar(label, size, data_type, p_data, p_min, p_max, format, flags);
			}*/
		));
	}

    {	// Widgets: Input with Keyboard
		imgui_table.set_function("InputText", sol::overload(
			[](const char* label, const String& buf, int buf_size) {
				char* pBuffer = (char*)malloc(sizeof(char) * (buf_size));
				memset(pBuffer, 0, buf_size);
				size_t copySize = buf.size() < buf_size ? buf.size() : buf_size;
				memcpy(pBuffer, buf.c_str(), copySize);
				bool binput = ImGui::InputText(label, pBuffer, buf_size, 0);
				std::string text(pBuffer);
				free(pBuffer);
				return std::make_tuple(binput, text);
			},
			[](const char* label, const String& buf, int buf_size, ImGuiInputTextFlags flags) {
				char* pBuffer = (char*)malloc(sizeof(char) * (buf_size));
				memset(pBuffer, 0, buf_size);
				size_t copySize = buf.size() < buf_size ? buf.size() : buf_size;
				memcpy(pBuffer, buf.c_str(), copySize);
				bool binput = ImGui::InputText(label, pBuffer, buf_size, flags);
				std::string text(pBuffer);
				free(pBuffer);
				return std::make_tuple(binput, text);
			}
		));
		imgui_table.set_function("InputTextMultiline", sol::overload(
			[](const char* label, char* buf, size_t buf_size) ->bool {
				return ImGui::InputTextMultiline(label, buf, buf_size, ImVec2(0, 0), 0);
			},
			[](const char* label, char* buf, size_t buf_size, const ImVec2& size) ->bool {
				return ImGui::InputTextMultiline(label, buf, buf_size, size, 0);
			},
			[](const char* label, char* buf, size_t buf_size, const ImVec2& size, ImGuiInputTextFlags flags) ->bool {
				return ImGui::InputTextMultiline(label, buf, buf_size, size, flags);
			}
		));
		imgui_table.set_function("InputTextWithHint", sol::overload(
			[](const char* label,  const char* hint, char* buf, size_t buf_size) ->bool {
				return ImGui::InputTextWithHint(label, hint, buf, buf_size, 0);
			},
			[](const char* label,  const char* hint, char* buf, size_t buf_size, ImGuiInputTextFlags flags) ->bool {
				return ImGui::InputTextWithHint(label, hint, buf, buf_size, flags);
			}
		));
		imgui_table.set_function("InputFloat", sol::overload(
			[](const char* label, float* v) ->bool {
				return ImGui::InputFloat(label, v, 0.0f, 0.0f, "%.3f", 0);
			},
			[](const char* label, float* v, float step) ->bool {
				return ImGui::InputFloat(label, v, step, 0.0f, "%.3f", 0);
			},
			[](const char* label, float* v, float step, float step_fast) ->bool {
				return ImGui::InputFloat(label, v, step, step_fast, "%.3f", 0);
			},
			[](const char* label, float* v, float step, float step_fast, const char* format) ->bool {
				return ImGui::InputFloat(label, v, step, step_fast, format, 0);
			},
			[](const char* label, float* v, float step, float step_fast, const char* format, ImGuiInputTextFlags flags) ->bool {
				return ImGui::InputFloat(label, v, step, step_fast, format, flags);
			}
		));
		imgui_table.set_function("InputFloat2", sol::overload(
			[](const char* label, float v[2]) ->bool {
				return ImGui::InputFloat2(label, v, "%.3f", 0);
			},
			[](const char* label, float v[2], const char* format) ->bool {
				return ImGui::InputFloat2(label, v, format, 0);
			},
			[](const char* label, float v[2], const char* format, ImGuiInputTextFlags flags) ->bool {
				return ImGui::InputFloat2(label, v, format, flags);
			}
		));
		imgui_table.set_function("InputFloat3", sol::overload(
			[](const char* label, float v[3]) ->bool {
				return ImGui::InputFloat3(label, v, "%.3f", 0);
			},
			[](const char* label, float v[3], const char* format) ->bool {
				return ImGui::InputFloat3(label, v, format, 0);
			},
			[](const char* label, float v[3], const char* format, ImGuiInputTextFlags flags) ->bool {
				return ImGui::InputFloat3(label, v, format, flags);
			}
		));
		imgui_table.set_function("InputFloat4", sol::overload(
			[](const char* label, float v[4]) ->bool {
				return ImGui::InputFloat4(label, v, "%.3f", 0);
			},
			[](const char* label, float v[4], const char* format) ->bool {
				return ImGui::InputFloat4(label, v, format, 0);
			},
			[](const char* label, float v[4], const char* format, ImGuiInputTextFlags flags) ->bool {
				return ImGui::InputFloat4(label, v, format, flags);
			}
		));
		imgui_table.set_function("InputInt", sol::overload(
			[](const char* label, int* v) ->bool {
				return ImGui::InputInt(label, v, 1, 100, 0);
			},
			[](const char* label, int* v, int step) ->bool {
				return ImGui::InputInt(label, v, step, 100, 0);
			},
			[](const char* label, int* v, int step, int step_fast) ->bool {
				return ImGui::InputInt(label, v, step, step_fast, 0);
			},
			[](const char* label, int* v, int step, int step_fast, ImGuiInputTextFlags flags) ->bool {
				return ImGui::InputInt(label, v, step, step_fast, flags);
			}
		));
		imgui_table.set_function("InputInt2", sol::overload(
			[](const char* label, int v[2]) ->bool {
				return ImGui::InputInt2(label, v, 0);
			},
			[](const char* label, int v[2], ImGuiInputTextFlags flags) ->bool {
				return ImGui::InputInt2(label, v, flags);
			}
		));
		imgui_table.set_function("InputInt3", sol::overload(
			[](const char* label, int v[3]) ->bool {
				return ImGui::InputInt3(label, v, 0);
			},
			[](const char* label, int v[3], ImGuiInputTextFlags flags) ->bool {
				return ImGui::InputInt3(label, v, flags);
			}
		));
		imgui_table.set_function("InputInt4", sol::overload(
			[](const char* label, int v[4]) ->bool {
				return ImGui::InputInt4(label, v, 0);
			},
			[](const char* label, int v[4], ImGuiInputTextFlags flags) ->bool {
				return ImGui::InputInt4(label, v, flags);
			}
		));
		imgui_table.set_function("InputDouble", sol::overload(
			[](const char* label, double* v) ->bool {
				return ImGui::InputDouble(label, v, 0.0, 0.0, "%.6f", 0);
			},
			[](const char* label, double* v, double step) ->bool {
				return ImGui::InputDouble(label, v, step, 0.0, "%.6f", 0);
			},
			[](const char* label, double* v, double step, double step_fast) ->bool {
				return ImGui::InputDouble(label, v, step, step_fast, "%.6f", 0);
			},
			[](const char* label, double* v, double step, double step_fast, const char* format) ->bool {
				return ImGui::InputDouble(label, v, step, step_fast, format, 0);
			},
			[](const char* label, double* v, double step, double step_fast, const char* format, ImGuiInputTextFlags flags) ->bool {
				return ImGui::InputDouble(label, v, step, step_fast, format, flags);
			}
		));
		imgui_table.set_function("InputScalar", sol::overload(
			[](const char* label, ImGuiDataType data_type,  void* p_data) ->bool {
				return ImGui::InputScalar(label, data_type, p_data, NULL, NULL, NULL, 0);
			},
			[](const char* label, ImGuiDataType data_type,  void* p_data, const void* p_step) ->bool {
				return ImGui::InputScalar(label, data_type, p_data, p_step, NULL, NULL, 0);
			},
			[](const char* label, ImGuiDataType data_type,  void* p_data, const void* p_step, const void* p_step_fast) ->bool {
				return ImGui::InputScalar(label, data_type, p_data, p_step, p_step_fast, NULL, 0);
			},
			[](const char* label, ImGuiDataType data_type,  void* p_data, const void* p_step, const void* p_step_fast, const char* format) ->bool {
				return ImGui::InputScalar(label, data_type, p_data, p_step, p_step_fast, format, 0);
			},
			[](const char* label, ImGuiDataType data_type,  void* p_data, const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags) ->bool {
				return ImGui::InputScalar(label, data_type, p_data, p_step, p_step_fast, format, flags);
			}
		));
		imgui_table.set_function("InputScalarN", sol::overload(
			[](const char* label, ImGuiDataType data_type,  void* p_data, int components) ->bool {
				return ImGui::InputScalarN(label, data_type, p_data, components, NULL, NULL, NULL, 0);
			},
			[](const char* label, ImGuiDataType data_type,  void* p_data, int components, const void* p_step) ->bool {
				return ImGui::InputScalarN(label, data_type, p_data, components, p_step, NULL, NULL, 0);
			},
			[](const char* label, ImGuiDataType data_type,  void* p_data, int components, const void* p_step, const void* p_step_fast) ->bool {
				return ImGui::InputScalarN(label, data_type, p_data, components, p_step, p_step_fast, NULL, 0);
			},
			[](const char* label, ImGuiDataType data_type,  void* p_data, int components, const void* p_step, const void* p_step_fast, const char* format) ->bool {
				return ImGui::InputScalarN(label, data_type, p_data, components, p_step, p_step_fast, format, 0);
			},
			[](const char* label, ImGuiDataType data_type,  void* p_data, int components, const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags) ->bool {
				return ImGui::InputScalarN(label, data_type, p_data, components, p_step, p_step_fast, format, flags);
			}
		));
	}

	{	// Widgets: Color Editor/Picker 
		imgui_table.set_function("ColorEdit3", sol::overload(
			[](const char* label, float col[3]) ->bool {
				return ImGui::ColorEdit3(label, col, 0);
			},
			[](const char* label, float col[3], ImGuiInputTextFlags flags) ->bool {
				return ImGui::ColorEdit3(label, col, flags);
			}
		));
		imgui_table.set_function("ColorEdit4", sol::overload(
			[](const char* label, float col[4]) ->bool {
				return ImGui::ColorEdit4(label, col, 0);
			},
			[](const char* label, float col[4], ImGuiInputTextFlags flags) ->bool {
				return ImGui::ColorEdit4(label, col, flags);
			}
		));
		imgui_table.set_function("ColorPicker3", sol::overload(
			[](const char* label, float col[3]) ->bool {
				return ImGui::ColorPicker3(label, col, 0);
			},
			[](const char* label, float col[3], ImGuiInputTextFlags flags) ->bool {
				return ImGui::ColorPicker3(label, col, flags);
			}
		));
		imgui_table.set_function("ColorPicker4", sol::overload(
			[](const char* label, float col[4]) ->bool {
				return ImGui::ColorPicker4(label, col, 0, NULL);
			},
			[](const char* label, float col[4], ImGuiInputTextFlags flags) ->bool {
				return ImGui::ColorPicker4(label, col, flags, NULL);
			},
			[](const char* label, float col[4], ImGuiInputTextFlags flags, const float* ref_col) ->bool {
				return ImGui::ColorPicker4(label, col, flags, ref_col);
			}
		));
		imgui_table.set_function("ColorButton", sol::overload(
			[](const char* desc_id, const ImVec4& col) ->bool {
				return ImGui::ColorButton(desc_id, col, 0, ImVec2(0, 0));
			},
			[](const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags) ->bool {
				return ImGui::ColorButton(desc_id, col, flags, ImVec2(0, 0));
			},
			[](const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags, const ImVec2& size) ->bool {
				return ImGui::ColorButton(desc_id, col, flags, size);
			}
		));
		imgui_table.set_function("SetColorEditOptions", sol::overload(
			[](ImGuiInputTextFlags flags) {
				return ImGui::SetColorEditOptions(flags);
			}
		));
	}

	{	// Widgets: Trees
		imgui_table.set_function("TreeNode", sol::overload(
			[](const char* label) ->bool {
				return ImGui::TreeNode(label);
			},
			[](const char* label, const char* fmt) ->bool {
				return ImGui::TreeNode(label, fmt);
			}
		));
		imgui_table.set_function("TreeNodeEx", sol::overload(
			[](const char* label) ->bool {
				return ImGui::TreeNodeEx(label, 0);
			},
			[](const char* label, ImGuiTreeNodeFlags flags) ->bool {
				return ImGui::TreeNodeEx(label, flags);
			},
			[](const char* label, ImGuiTreeNodeFlags flags, const char* fmt) ->bool {
				return ImGui::TreeNodeEx(label, flags, fmt);
			}
		));
		imgui_table.set_function("TreePush", sol::overload(
			[](const char* str_id) {
				return ImGui::TreePush(str_id);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(TreePop));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(GetTreeNodeToLabelSpacing));
		imgui_table.set_function("CollapsingHeader", sol::overload(
			[](const char* label) ->bool {
				return ImGui::CollapsingHeader(label, 0);
			},
			[](const char* label, ImGuiTreeNodeFlags flags) ->bool {
				return ImGui::CollapsingHeader(label, flags);
			},
			[](const char* label, bool p_visible) ->bool {
				return ImGui::CollapsingHeader(label, &p_visible, 0);
			},
			[](const char* label, bool p_visible, ImGuiTreeNodeFlags flags) ->bool {
				return ImGui::CollapsingHeader(label, &p_visible, flags);
			}
		));
		imgui_table.set_function("SetNextItemOpen", sol::overload(
			[](bool is_open) { return ImGui::SetNextItemOpen(is_open, 0); },
			[](bool is_open, ImGuiCond cond) { return ImGui::SetNextItemOpen(is_open, cond); }
		));
	}

	{	// Widgets: Selectables
		imgui_table.set_function("Selectable", sol::overload(
			[](const char* label) ->bool {
				return ImGui::Selectable(label, false, 0, ImVec2(0, 0));
			},
			[](const char* label, bool selected) ->bool {
				return ImGui::Selectable(label, selected, 0, ImVec2(0, 0));
			},
			[](const char* label, bool selected, ImGuiSelectableFlags flags) ->bool {
				return ImGui::Selectable(label, selected, flags, ImVec2(0, 0));
			},
			[](const char* label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size) ->bool {
				return ImGui::Selectable(label, selected, flags, size);
			}
		));
	}

	{	// Widgets: List Boxes
		/*imgui_table.set_function("BeginListBox", sol::overload(
			[](const char* label) ->bool {
				return ImGui::BeginListBox(label, ImVec2(0, 0));
			},
			[](const char* label, const ImVec2& size) ->bool {
				return ImGui::BeginListBox(label, size);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(EndListBox));*/
		imgui_table.set_function("ListBox", sol::overload(
			[](const char* label, int* current_item, const char* const items[], int items_count) ->bool {
				return ImGui::ListBox(label, current_item, items, items_count, -1);
			},
			[](const char* label, int* current_item, const char* const items[], int items_count, int height_in_items) ->bool {
				return ImGui::ListBox(label, current_item, items, items_count, height_in_items);
			}
		));
	}

	{	// Widgets: Data Plotting
		imgui_table.set_function("PlotLines", sol::overload(
			[](const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, 
				float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float)) {
				return ImGui::PlotLines(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, stride);
			}
		));
		imgui_table.set_function("PlotHistogram", sol::overload(
			[](const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, 
				float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float)) {
				return ImGui::PlotHistogram(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, stride);
			}
		));
	}

	{	// Widgets: Value() Helpers.
		imgui_table.set_function("Value", sol::overload(
			[](const char* prefix, bool b) { return ImGui::Value(prefix, b); },
			[](const char* prefix, int v) { return ImGui::Value(prefix, v); },
			[](const char* prefix, unsigned int v) { return ImGui::Value(prefix, v); },
			[](const char* prefix, float v, const char* float_format = NULL) { return ImGui::Value(prefix, v, float_format); }
		));
	}

	{	// Widgets: Menus
		imgui_table.set_function(REGISTER_IMGUI_FUNC(BeginMenuBar));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(EndMenuBar));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(BeginMainMenuBar));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(EndMainMenuBar));
		imgui_table.set_function("MenuItem", sol::overload(
			[](const char* label, const char* shortcut = NULL, bool selected = false, bool enabled = true) ->bool {
				return ImGui::MenuItem(label, shortcut, selected, enabled);
			},
			[](const char* label, const char* shortcut, bool p_selected, bool enabled = true) ->bool {
				return ImGui::MenuItem(label, shortcut, &p_selected, enabled);
			}
			));
		imgui_table.set_function("BeginMenu", sol::overload(
			[](const char* label) ->bool { return ImGui::BeginMenu(label); },
			[](const char* label, bool enabled) ->bool { return ImGui::BeginMenu(label, enabled); }
			));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(EndMenu));
	}

	{	// Widgets: Tooltips
		imgui_table.set_function(REGISTER_IMGUI_FUNC(BeginTooltip));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(EndTooltip));
		imgui_table.set_function("SetTooltip", sol::overload(
			[](const char* fmt) {
				return ImGui::SetTooltip(fmt);
			}
		));
	}

	{	// Popups, Modals
		imgui_table.set_function("BeginPopup", sol::overload(
			[](const char* str_id, ImGuiWindowFlags flags) ->bool {
				return ImGui::BeginPopup(str_id, flags);
			}
		));
		imgui_table.set_function("BeginPopupModal", sol::overload(
			[](const char* name) {
				return std::make_tuple(ImGui::BeginPopupModal(name, NULL, 0), false);
			},
			[](const char* name, bool p_open) {
				bool b_open = p_open;
				bool bPopup = ImGui::BeginPopupModal(name, &b_open, 0);
				return std::make_tuple(bPopup, b_open);
			},
			[](const char* name, bool p_open, ImGuiWindowFlags flags) {
				bool b_open = p_open;
				bool bPopup = ImGui::BeginPopupModal(name, &b_open, flags);
				return std::make_tuple(bPopup, b_open);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(EndPopup));
	}

	{	// Popups: begin/end functions
		imgui_table.set_function("OpenPopup", sol::overload(
			[](const char* str_id) { return ImGui::OpenPopup(str_id); }/*,
			[](const char* str_id, ImGuiPopupFlags popup_flags) { return ImGui::OpenPopup(str_id, popup_flags); },
			[](ImGuiID id) { return ImGui::OpenPopup(id); },
			[](ImGuiID id, ImGuiPopupFlags popup_flags) { return ImGui::OpenPopup(id, popup_flags); }*/
		));
		/*imgui_table.set_function("OpenPopupOnItemClick", sol::overload(
			[](const char* str_id = NULL, ImGuiPopupFlags popup_flags = 1) {
				return ImGui::OpenPopupOnItemClick(str_id, popup_flags);
			}
		));*/
		imgui_table.set_function(REGISTER_IMGUI_FUNC(CloseCurrentPopup));
	}

	{	// Popups: open+begin combined functions helpers
		imgui_table.set_function("BeginPopupContextItem", sol::overload(
			[]() -> bool { return ImGui::BeginPopupContextItem(); },
			[](const char* str_id) -> bool { return ImGui::BeginPopupContextItem(str_id); },
			[](const char* str_id, int popup_flags) -> bool { return ImGui::BeginPopupContextItem(str_id, popup_flags); }
		));
		imgui_table.set_function("BeginPopupContextWindow", sol::overload(
			[]() -> bool { return ImGui::BeginPopupContextWindow(); },
			[](const char* str_id) -> bool { return ImGui::BeginPopupContextWindow(str_id); },
			[](const char* str_id, int popup_flags) -> bool { return ImGui::BeginPopupContextWindow(str_id, popup_flags); }
		));
		imgui_table.set_function("BeginPopupContextVoid", sol::overload(
			[]() -> bool { return ImGui::BeginPopupContextVoid(); },
			[](const char* str_id) -> bool { return ImGui::BeginPopupContextVoid(str_id); },
			[](const char* str_id, int popup_flags) -> bool { return ImGui::BeginPopupContextVoid(str_id, popup_flags); }
		));
	}

	{	// Popups: query functions
		imgui_table.set_function("IsPopupOpen", sol::overload(
			[]() -> bool {
				return ImGui::BeginPopupContextItem(NULL, 1);
			},
			[](const char* str_id) -> bool {
				return ImGui::BeginPopupContextItem(str_id, 1);
			},
			[](const char* str_id, int flags) -> bool {
				return ImGui::BeginPopupContextItem(str_id, flags);
			}
		));
	}

	/*{	// Tables
		imgui_table.set_function("BeginTable", sol::overload(
			[](const char* str_id, int column, ImGuiTableFlags flags, const ImVec2& outer_size = ImVec2(0.0f, 0.0f), float inner_width = 0.0f) -> bool {
				return ImGui::BeginTable(str_id, column, flags, outer_size, inner_width);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(EndTable));
		imgui_table.set_function("TableNextRow", sol::overload(
			[](ImGuiTableRowFlags row_flags = 0, float min_row_height = 0.0f) {
				return ImGui::TableNextRow(row_flags, min_row_height);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(TableNextColumn));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(TableSetColumnIndex));
	}

	{	// Tables: Headers & Columns declaration
		imgui_table.set_function("TableSetupColumn", sol::overload(
			[](const char* label, ImGuiTableColumnFlags flags, float init_width_or_weight = 0.0f, ImGuiID user_id = 0) {
				return ImGui::TableSetupColumn(label, flags, init_width_or_weight, user_id);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(TableSetupScrollFreeze));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(TableHeadersRow));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(TableHeader));
	}

	// Tables: Sorting & Miscellaneous functions

	{	// Legacy Columns API (prefer using Tables!)
		imgui_table.set_function("Columns", sol::overload(
			[](int count = 1, const char* id = NULL, bool border = true) {
				return ImGui::Columns(count, id, border);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(NextColumn));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(GetColumnIndex));
		imgui_table.set_function("GetColumnWidth", sol::overload(
			[](int column_index = -1) {
				return ImGui::GetColumnWidth(column_index);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(SetColumnWidth));
		imgui_table.set_function("GetColumnOffset", sol::overload(
			[](int column_index = -1) {
				return ImGui::GetColumnOffset(column_index);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(SetColumnOffset));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(GetColumnsCount));
	}

	{	// Tab Bars, Tabs
		imgui_table.set_function("BeginTabBar", sol::overload(
			[](const char* str_id, ImGuiTabBarFlags flags) {
				return ImGui::BeginTabBar(str_id, flags);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(EndTabBar));
		imgui_table.set_function("BeginTabItem", sol::overload(
			[](const char* label, bool p_open, ImGuiTabItemFlags flags) {
				return ImGui::BeginTabItem(label, &p_open, flags);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(EndTabItem));
		imgui_table.set_function("TabItemButton", sol::overload(
			[](const char* label, ImGuiTabItemFlags flags) {
				return ImGui::TabItemButton(label, flags);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(SetTabItemClosed));
	}*/

	{	// Docking
		imgui_table.set_function("DockSpace", sol::overload(
			[](ImGuiID id) {
				return ImGui::DockSpace(id, ImVec2(0, 0), 0, NULL);
			},
			[](ImGuiID id, const ImVec2& size) {
				return ImGui::DockSpace(id, size, 0, NULL);
			},
			[](ImGuiID id, const ImVec2& size, ImGuiDockNodeFlags flags) {
				return ImGui::DockSpace(id, size, flags, NULL);
			},
			[](ImGuiID id, const ImVec2& size, ImGuiDockNodeFlags flags, const ImGuiWindowClass* window_class) {
				return ImGui::DockSpace(id, size, flags, window_class);
			}
		));
		imgui_table.set_function("DockSpaceOverViewport", sol::overload(
			[]() -> ImGuiID {
				return ImGui::DockSpaceOverViewport(NULL, 0, NULL);
			},
			[](ImGuiViewport* viewport) -> ImGuiID {
				return ImGui::DockSpaceOverViewport(viewport, 0, NULL);
			},
			[](ImGuiViewport* viewport, ImGuiDockNodeFlags flags) -> ImGuiID {
				return ImGui::DockSpaceOverViewport(viewport, flags, NULL);
			},
			[](ImGuiViewport* viewport, ImGuiDockNodeFlags flags, const ImGuiWindowClass* window_class) -> ImGuiID {
				return ImGui::DockSpaceOverViewport(viewport, flags, window_class);
			}
		));
		imgui_table.set_function("SetNextWindowDockID", sol::overload(
			[](ImGuiID dock_id, ImGuiCond cond = 0) {
				return ImGui::SetNextWindowDockID(dock_id, cond);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(SetNextWindowClass));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(GetWindowDockID));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(IsWindowDocked));
	}

	{	// Logging/Capture
		imgui_table.set_function("LogToTTY", sol::overload(
			[](int auto_open_depth = -1) {
				return ImGui::LogToTTY(auto_open_depth);
			}
		));
		imgui_table.set_function("LogToFile", sol::overload(
			[](int auto_open_depth = -1, const char* filename = NULL) {
				return ImGui::LogToFile(auto_open_depth, filename);
			}
		));
		imgui_table.set_function("LogToClipboard", sol::overload(
			[](int auto_open_depth = -1) {
				return ImGui::LogToClipboard(auto_open_depth);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(LogFinish));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(LogButtons));
		imgui_table.set_function("LogText", sol::overload(
			[](const char* fmt) {
				return ImGui::LogText(fmt);
			}
		));
	}

	{	// Drag and Drop
		imgui_table.set_function("BeginDragDropSource", sol::overload(
			[](ImGuiDragDropFlags flags) {
				return ImGui::BeginDragDropSource(flags);
			}
		));
		imgui_table.set_function("SetDragDropPayload", sol::overload(
			[](const char* type, const void* data, size_t sz, ImGuiCond cond = 0) {
				return ImGui::SetDragDropPayload(type, data, sz, cond);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(EndDragDropSource));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(BeginDragDropTarget));
		imgui_table.set_function("AcceptDragDropPayload", sol::overload(
			[](const char* type, ImGuiDragDropFlags flags) {
				return ImGui::AcceptDragDropPayload(type, flags);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(EndDragDropTarget));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(GetDragDropPayload));
	}

	/*{	// Disabling [BETA API]
		imgui_table.set_function("BeginDisabled", sol::overload(
			[](bool disabled = true) {
				return ImGui::BeginDisabled(disabled);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(EndDisabled));
	}*/

	{	// Clipping 
		imgui_table.set_function(REGISTER_IMGUI_FUNC(PushClipRect));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(PopClipRect));
	}

	{	// Focus, Activation
		imgui_table.set_function(REGISTER_IMGUI_FUNC(SetItemDefaultFocus));
		imgui_table.set_function("SetKeyboardFocusHere", sol::overload(
			[](int offset = 0) {
				return ImGui::SetKeyboardFocusHere(offset);
			}
		));
	}

	{	// Focus, Activation
		imgui_table.set_function("IsItemHovered", sol::overload(
			[](ImGuiHoveredFlags flags) {
				return ImGui::IsItemHovered(flags);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(IsItemActive));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(IsItemFocused));
		imgui_table.set_function("IsItemClicked", sol::overload(
			[](int mouse_button = 0) {
				return ImGui::IsItemClicked(mouse_button);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(IsItemVisible));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(IsItemEdited));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(IsItemActivated));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(IsItemDeactivated));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(IsItemDeactivatedAfterEdit));
		//imgui_table.set_function(REGISTER_IMGUI_FUNC(IsItemToggledOpen));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(IsAnyItemHovered));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(IsAnyItemActive));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(IsAnyItemFocused));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(GetItemRectMin));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(GetItemRectMax));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(GetItemRectSize));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(SetItemAllowOverlap));
	}

	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetMainViewport));

	{	// Background/Foreground Draw Lists	
		imgui_table.set_function("GetBackgroundDrawList", sol::overload(
			[]() {
				return ImGui::GetBackgroundDrawList();
			},
			[](ImGuiViewport* viewport) {
				return ImGui::GetBackgroundDrawList(viewport);
			}
		));
		imgui_table.set_function("GetForegroundDrawList", sol::overload(
			[]() {
				return ImGui::GetForegroundDrawList();
			},
			[](ImGuiViewport* viewport) {
				return ImGui::GetForegroundDrawList(viewport);
			}
		));
	}

	{	// Miscellaneous Utilities
		imgui_table.set_function("IsRectVisible", sol::overload(
			[](const ImVec2& size) -> bool {
				return ImGui::IsRectVisible(size);
			},
			[](const ImVec2& rect_min, const ImVec2& rect_max) -> bool {
				return ImGui::IsRectVisible(rect_min, rect_max);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(GetTime));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(GetFrameCount));
		//imgui_table.set_function(REGISTER_IMGUI_FUNC(GetDrawListSharedData));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(GetStyleColorName));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(SetStateStorage));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(GetStateStorage));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(BeginChildFrame));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(EndChildFrame));
	}

	imgui_table.set_function("CalcTextSize", sol::overload(
			[](const char* text, const char* text_end = NULL, 
				bool hide_text_after_double_hash = false, float wrap_width = -1.0f) -> ImVec2 {
				return ImGui::CalcTextSize(text, text_end, hide_text_after_double_hash, wrap_width);
			}
		));

	{	// Color Utilities
		imgui_table.set_function(REGISTER_IMGUI_FUNC(ColorConvertU32ToFloat4));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(ColorConvertFloat4ToU32));
		imgui_table.set_function("ColorConvertRGBtoHSV", sol::overload(
			[](float r, float g, float b) -> Math::Vec3* {
				Math::Vec3* res = _NEW Math::Vec3;
				ImGui::ColorConvertRGBtoHSV(r, g, b, res->x, res->y, res->z);
				return res;
			}
		));
		imgui_table.set_function("ColorConvertHSVtoRGB", sol::overload(
			[](float h, float s, float v) -> Math::Vec3* {
				Math::Vec3* res = _NEW Math::Vec3;
				ImGui::ColorConvertHSVtoRGB(h, s, v, res->x, res->y, res->z);
				return res;
			}
		));
	}

	{	// Inputs Utilities: Keyboard	
		imgui_table.set_function(REGISTER_IMGUI_FUNC(IsKeyDown));
		imgui_table.set_function("IsKeyPressed", sol::overload(
			[](ImGuiKey key, bool repeat = true) -> bool {
				return ImGui::IsKeyPressed(key, repeat);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(IsKeyReleased));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(GetKeyPressedAmount));
		//imgui_table.set_function(REGISTER_IMGUI_FUNC(GetKeyName));
		//imgui_table.set_function(REGISTER_IMGUI_FUNC(SetNextFrameWantCaptureKeyboard));
	}

	{	// Inputs Utilities: Mouse
		imgui_table.set_function(REGISTER_IMGUI_FUNC(IsMouseDown));
		imgui_table.set_function("IsItemHovered", sol::overload(
			[](ImGuiHoveredFlags flags) -> bool {
				return ImGui::IsItemHovered(flags);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(IsMouseReleased));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(IsMouseDoubleClicked));
		//imgui_table.set_function(REGISTER_IMGUI_FUNC(GetMouseClickedCount));
		imgui_table.set_function("IsMouseHoveringRect", sol::overload(
			[](const ImVec2& r_min, const ImVec2& r_max, bool clip = true) -> bool {
				return ImGui::IsMouseHoveringRect(r_min, r_max, clip);
			}
		));
		imgui_table.set_function("IsMousePosValid", sol::overload(
			[](const ImVec2* mouse_pos = NULL) -> bool {
				return ImGui::IsMousePosValid(mouse_pos);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(IsAnyMouseDown));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(GetMousePos));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(GetMousePosOnOpeningCurrentPopup));
		imgui_table.set_function("IsMouseDragging", sol::overload(
			[]() -> bool {
				return ImGui::IsMouseDragging();
			},
			[](int button) -> bool {
				return ImGui::IsMouseDragging(button);
			},
			[](int button, float lock_threshold) -> bool {
				return ImGui::IsMouseDragging(button, lock_threshold);
			}
		));
		imgui_table.set_function("GetMouseDragDelta", sol::overload(
			[]() { return ImGui::GetMouseDragDelta(); },
			[](int button) { return ImGui::GetMouseDragDelta(button); },
			[](int button, float lock_threshold) { return ImGui::GetMouseDragDelta(button, lock_threshold); }
		));
		imgui_table.set_function("ResetMouseDragDelta", sol::overload(
			[]() { return ImGui::ResetMouseDragDelta(); },
			[](int button) { return ImGui::ResetMouseDragDelta(button); }
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(GetMouseCursor));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(SetMouseCursor));
		//imgui_table.set_function(REGISTER_IMGUI_FUNC(SetNextFrameWantCaptureMouse));
	}

	{	// Clipboard Utilities
		imgui_table.set_function(REGISTER_IMGUI_FUNC(GetClipboardText));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(SetClipboardText));
	}

	{	// Settings/.Ini Utilities
		imgui_table.set_function(REGISTER_IMGUI_FUNC(LoadIniSettingsFromDisk));
		imgui_table.set_function("LoadIniSettingsFromMemory", sol::overload(
			[](const char* ini_data, size_t ini_size=0) {
				return ImGui::LoadIniSettingsFromMemory(ini_data, ini_size);
			}
		));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(SaveIniSettingsToDisk));
		imgui_table.set_function("SaveIniSettingsToMemory", sol::overload(
			[](size_t* out_ini_size = NULL) {
				return ImGui::SaveIniSettingsToMemory(out_ini_size);
			}
		));
	}

	{	// Debug Utilities
		//imgui_table.set_function(REGISTER_IMGUI_FUNC(DebugTextEncoding));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(DebugCheckVersionAndDataLayout));
	}

	{	// (Optional) Platform/OS interface for multi-viewport support
		imgui_table.set_function(REGISTER_IMGUI_FUNC(GetPlatformIO));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(UpdatePlatformWindows));
		imgui_table.set_function(REGISTER_IMGUI_FUNC(DestroyPlatformWindows));
		//imgui_table.set_function(REGISTER_IMGUI_FUNC(FindViewportByID));
	}

	{	// imgui_user.h
		imgui_table.set_function("ImageButtonEx", sol::overload(
			[](TextureEntity* user_texture_id, const ImVec2& size, const String& tip, bool select, bool enable) {
				return ImGui::ImageButtonEx(user_texture_id, size, tip.c_str(), select, enable);
			}
		));
		imgui_table.set_function("ToolbarButtonEx", sol::overload(
			[](TextureEntity* user_texture_id, const String& tip, bool select, bool enable) {
				return ImGui::ToolbarButtonEx(user_texture_id, tip.c_str(), select, enable);
			}
		));
	}
		
	imgui_table.set_function(REGISTER_IMGUI_FUNC(GetKeyIndex));

	{	// 一些自定义的函数
		imgui_table.set_function("GetMouseWheel", sol::overload(
			[]() {
				ImGuiIO io = ImGui::GetIO();
				return std::make_tuple(io.MouseWheel, io.MouseWheelH);
			}
		));
		imgui_table.set_function("CircleLoadingBar", sol::overload(
			[]() {
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
		));
	}

    #define REGISTER_IMGUI_ENUM(flags) imgui_table[#flags] = flags;

	{	// Flags for ImGui::Begin()
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_None);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_NoTitleBar);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_NoResize);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_NoMove);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_NoScrollbar);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_NoScrollWithMouse);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_NoCollapse);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_AlwaysAutoResize);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_NoBackground);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_NoSavedSettings);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_NoMouseInputs);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_MenuBar);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_HorizontalScrollbar);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_NoFocusOnAppearing);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_NoBringToFrontOnFocus);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_AlwaysVerticalScrollbar);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_AlwaysHorizontalScrollbar);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_AlwaysUseWindowPadding);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_NoNavInputs);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_NoNavFocus);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_UnsavedDocument);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_NoDocking);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_NoNav);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_NoDecoration);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_NoInputs);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_NavFlattened);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_ChildWindow);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_Tooltip);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_Popup);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_Modal);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_ChildMenu);
		REGISTER_IMGUI_ENUM(ImGuiWindowFlags_DockNodeHost);
	}

	{	// Flags for ImGui::InputText()
		REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_None);
		REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_CharsDecimal);
		REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_CharsHexadecimal);
		REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_CharsUppercase);
		REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_CharsNoBlank);
		REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_AutoSelectAll);
		REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_EnterReturnsTrue);
		REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_CallbackCompletion);
		REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_CallbackHistory);
		REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_CallbackAlways);
		REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_CallbackCharFilter);
		REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_AllowTabInput);
		REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_CtrlEnterForNewLine);
		REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_NoHorizontalScroll);
		//REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_AlwaysOverwrite);
		REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_ReadOnly);
		REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_Password);
		REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_NoUndoRedo);
		REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_CharsScientific);
		REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_CallbackResize);
		//REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_CallbackEdit);
		//REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_EscapeClearsAll);
		//REGISTER_IMGUI_ENUM(ImGuiInputTextFlags_CallbackEdit);
	}

	{	// Flags for ImGui::TreeNodeEx(), ImGui::CollapsingHeader*()
		REGISTER_IMGUI_ENUM(ImGuiTreeNodeFlags_None);
		REGISTER_IMGUI_ENUM(ImGuiTreeNodeFlags_Selected);
		REGISTER_IMGUI_ENUM(ImGuiTreeNodeFlags_Framed);
		REGISTER_IMGUI_ENUM(ImGuiTreeNodeFlags_AllowItemOverlap);
		REGISTER_IMGUI_ENUM(ImGuiTreeNodeFlags_NoTreePushOnOpen);
		REGISTER_IMGUI_ENUM(ImGuiTreeNodeFlags_NoAutoOpenOnLog);
		REGISTER_IMGUI_ENUM(ImGuiTreeNodeFlags_DefaultOpen);
		REGISTER_IMGUI_ENUM(ImGuiTreeNodeFlags_OpenOnDoubleClick);
		REGISTER_IMGUI_ENUM(ImGuiTreeNodeFlags_OpenOnArrow);
		REGISTER_IMGUI_ENUM(ImGuiTreeNodeFlags_Leaf);
		REGISTER_IMGUI_ENUM(ImGuiTreeNodeFlags_Bullet);
		REGISTER_IMGUI_ENUM(ImGuiTreeNodeFlags_FramePadding);
		//REGISTER_IMGUI_ENUM(ImGuiTreeNodeFlags_SpanAvailWidth);
		//REGISTER_IMGUI_ENUM(ImGuiTreeNodeFlags_SpanFullWidth);
		REGISTER_IMGUI_ENUM(ImGuiTreeNodeFlags_NavLeftJumpsBackHere);
		REGISTER_IMGUI_ENUM(ImGuiTreeNodeFlags_CollapsingHeader);
	}

	{	// Flags for OpenPopup*(), BeginPopupContext*(), IsPopupOpen() functions.
		//REGISTER_IMGUI_ENUM(ImGuiPopupFlags_None);
		//REGISTER_IMGUI_ENUM(ImGuiPopupFlags_MouseButtonLeft);
		//REGISTER_IMGUI_ENUM(ImGuiPopupFlags_MouseButtonRight);
		//REGISTER_IMGUI_ENUM(ImGuiPopupFlags_MouseButtonMiddle);
		//REGISTER_IMGUI_ENUM(ImGuiPopupFlags_MouseButtonMask_);
		//REGISTER_IMGUI_ENUM(ImGuiPopupFlags_MouseButtonDefault_);
		//REGISTER_IMGUI_ENUM(ImGuiPopupFlags_NoOpenOverExistingPopup);
		//REGISTER_IMGUI_ENUM(ImGuiPopupFlags_NoOpenOverItems);
		//REGISTER_IMGUI_ENUM(ImGuiPopupFlags_AnyPopupId);
		//REGISTER_IMGUI_ENUM(ImGuiPopupFlags_AnyPopupLevel);
		//REGISTER_IMGUI_ENUM(ImGuiPopupFlags_AnyPopup);
	}

	{	// Flags for ImGui::Selectable()
		REGISTER_IMGUI_ENUM(ImGuiSelectableFlags_None);
		REGISTER_IMGUI_ENUM(ImGuiSelectableFlags_DontClosePopups);
		REGISTER_IMGUI_ENUM(ImGuiSelectableFlags_SpanAllColumns);
		REGISTER_IMGUI_ENUM(ImGuiSelectableFlags_AllowDoubleClick);
		REGISTER_IMGUI_ENUM(ImGuiSelectableFlags_Disabled);
		REGISTER_IMGUI_ENUM(ImGuiSelectableFlags_AllowItemOverlap);
	}

	{	// Flags for ImGui::BeginCombo()
		REGISTER_IMGUI_ENUM(ImGuiComboFlags_None);
		REGISTER_IMGUI_ENUM(ImGuiComboFlags_PopupAlignLeft);
		REGISTER_IMGUI_ENUM(ImGuiComboFlags_HeightSmall);
		REGISTER_IMGUI_ENUM(ImGuiComboFlags_HeightRegular);
		REGISTER_IMGUI_ENUM(ImGuiComboFlags_HeightLarge);
		REGISTER_IMGUI_ENUM(ImGuiComboFlags_HeightLargest);
		REGISTER_IMGUI_ENUM(ImGuiComboFlags_NoArrowButton);
		REGISTER_IMGUI_ENUM(ImGuiComboFlags_NoPreview);
		REGISTER_IMGUI_ENUM(ImGuiComboFlags_HeightMask_);
	}

	{	// Flags for ImGui::BeginTabBar()
		REGISTER_IMGUI_ENUM(ImGuiTabBarFlags_None);
		REGISTER_IMGUI_ENUM(ImGuiTabBarFlags_Reorderable);
		REGISTER_IMGUI_ENUM(ImGuiTabBarFlags_AutoSelectNewTabs);
		REGISTER_IMGUI_ENUM(ImGuiTabBarFlags_TabListPopupButton);
		REGISTER_IMGUI_ENUM(ImGuiTabBarFlags_NoCloseWithMiddleMouseButton);
		REGISTER_IMGUI_ENUM(ImGuiTabBarFlags_NoTabListScrollingButtons);
		REGISTER_IMGUI_ENUM(ImGuiTabBarFlags_NoTooltip);
		REGISTER_IMGUI_ENUM(ImGuiTabBarFlags_FittingPolicyResizeDown);
		REGISTER_IMGUI_ENUM(ImGuiTabBarFlags_FittingPolicyScroll);
		REGISTER_IMGUI_ENUM(ImGuiTabBarFlags_FittingPolicyMask_);
		REGISTER_IMGUI_ENUM(ImGuiTabBarFlags_FittingPolicyDefault_);
	}

	{	// Flags for ImGui::BeginTabItem()
		REGISTER_IMGUI_ENUM(ImGuiTabItemFlags_None);
		REGISTER_IMGUI_ENUM(ImGuiTabItemFlags_UnsavedDocument);
		REGISTER_IMGUI_ENUM(ImGuiTabItemFlags_SetSelected);
		REGISTER_IMGUI_ENUM(ImGuiTabItemFlags_NoCloseWithMiddleMouseButton);
		REGISTER_IMGUI_ENUM(ImGuiTabItemFlags_NoPushId);
		//REGISTER_IMGUI_ENUM(ImGuiTabItemFlags_NoTooltip);
		//REGISTER_IMGUI_ENUM(ImGuiTabItemFlags_NoReorder);
		//REGISTER_IMGUI_ENUM(ImGuiTabItemFlags_Leading);
		//REGISTER_IMGUI_ENUM(ImGuiTabItemFlags_Trailing);
	}

	/* {	// Flags for ImGui::BeginTable()
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_None);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_Resizable);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_Reorderable);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_Hideable);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_Sortable);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_NoSavedSettings);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_ContextMenuInBody);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_RowBg);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_BordersInnerH);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_BordersOuterH);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_BordersInnerV);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_BordersOuterV);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_BordersH);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_BordersV);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_BordersInner);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_BordersOuter);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_Borders);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_NoBordersInBody);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_NoBordersInBodyUntilResize);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_SizingFixedFit);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_SizingFixedSame);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_SizingStretchProp);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_SizingStretchSame);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_NoHostExtendX);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_NoHostExtendY);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_NoKeepColumnsVisible);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_NoClip);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_PadOuterX);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_NoPadOuterX);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_NoPadInnerX);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_ScrollX);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_ScrollY);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_SortMulti);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_SortTristate);
		REGISTER_IMGUI_ENUM(ImGuiTableFlags_SizingMask_);
	}

	{	// Flags for ImGui::TableSetupColumn()
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_None);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_Disabled);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_DefaultHide);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_DefaultSort);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_WidthStretch);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_WidthFixed);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_NoResize);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_NoReorder);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_NoHide);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_NoClip);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_NoSort);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_NoSortAscending);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_NoSortDescending);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_NoHeaderLabel);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_NoHeaderWidth);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_PreferSortAscending);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_PreferSortDescending);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_IndentEnable);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_IndentDisable);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_IsEnabled);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_IsVisible);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_IsSorted);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_IsHovered);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_WidthMask_);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_IndentMask_);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_StatusMask_);
		REGISTER_IMGUI_ENUM(ImGuiTableColumnFlags_NoDirectResize_);
	}
	
	{	// Flags for ImGui::TableNextRow()
		REGISTER_IMGUI_ENUM(ImGuiTableRowFlags_None);
		REGISTER_IMGUI_ENUM(ImGuiTableRowFlags_Headers);
	}
	
	{	// Flags for ImGui::TableSetBgColor()
		REGISTER_IMGUI_ENUM(ImGuiTableBgTarget_None);
		REGISTER_IMGUI_ENUM(ImGuiTableBgTarget_RowBg0);
		REGISTER_IMGUI_ENUM(ImGuiTableBgTarget_RowBg1);
		REGISTER_IMGUI_ENUM(ImGuiTableBgTarget_CellBg);
	}*/
	
	{	// Flags for ImGui::IsWindowFocused()
		REGISTER_IMGUI_ENUM(ImGuiFocusedFlags_None);
		REGISTER_IMGUI_ENUM(ImGuiFocusedFlags_ChildWindows);
		REGISTER_IMGUI_ENUM(ImGuiFocusedFlags_RootWindow);
		REGISTER_IMGUI_ENUM(ImGuiFocusedFlags_AnyWindow);
		//REGISTER_IMGUI_ENUM(ImGuiFocusedFlags_NoPopupHierarchy);
		//REGISTER_IMGUI_ENUM(ImGuiFocusedFlags_DockHierarchy);
		REGISTER_IMGUI_ENUM(ImGuiFocusedFlags_RootAndChildWindows);
	}
	
	{	// Flags for ImGui::IsItemHovered(), ImGui::IsWindowHovered()
		REGISTER_IMGUI_ENUM(ImGuiHoveredFlags_None);
		REGISTER_IMGUI_ENUM(ImGuiHoveredFlags_ChildWindows);
		REGISTER_IMGUI_ENUM(ImGuiHoveredFlags_RootWindow);
		REGISTER_IMGUI_ENUM(ImGuiHoveredFlags_AnyWindow);
		//REGISTER_IMGUI_ENUM(ImGuiHoveredFlags_NoPopupHierarchy);
		//REGISTER_IMGUI_ENUM(ImGuiHoveredFlags_DockHierarchy);
		REGISTER_IMGUI_ENUM(ImGuiHoveredFlags_AllowWhenBlockedByPopup);
		REGISTER_IMGUI_ENUM(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
		REGISTER_IMGUI_ENUM(ImGuiHoveredFlags_AllowWhenOverlapped);
		REGISTER_IMGUI_ENUM(ImGuiHoveredFlags_AllowWhenDisabled);
		//REGISTER_IMGUI_ENUM(ImGuiHoveredFlags_NoNavOverride);
		REGISTER_IMGUI_ENUM(ImGuiHoveredFlags_RectOnly);
		REGISTER_IMGUI_ENUM(ImGuiHoveredFlags_RootAndChildWindows);
		//REGISTER_IMGUI_ENUM(ImGuiHoveredFlags_DelayShort);
		//REGISTER_IMGUI_ENUM(ImGuiHoveredFlags_NoSharedDelay);
	}
	
	{	// Flags for ImGui::DockSpace(), shared/inherited by child nodes.
		REGISTER_IMGUI_ENUM(ImGuiDockNodeFlags_None);
		REGISTER_IMGUI_ENUM(ImGuiDockNodeFlags_KeepAliveOnly);
		REGISTER_IMGUI_ENUM(ImGuiDockNodeFlags_NoDockingInCentralNode);
		REGISTER_IMGUI_ENUM(ImGuiDockNodeFlags_PassthruCentralNode);
		REGISTER_IMGUI_ENUM(ImGuiDockNodeFlags_NoSplit);
		REGISTER_IMGUI_ENUM(ImGuiDockNodeFlags_NoResize);
		REGISTER_IMGUI_ENUM(ImGuiDockNodeFlags_AutoHideTabBar);
	}
	
	{	// Flags for ImGui::BeginDragDropSource(), ImGui::AcceptDragDropPayload()
		REGISTER_IMGUI_ENUM(ImGuiDragDropFlags_None);
		REGISTER_IMGUI_ENUM(ImGuiDragDropFlags_SourceNoPreviewTooltip);
		REGISTER_IMGUI_ENUM(ImGuiDragDropFlags_SourceNoDisableHover);
		REGISTER_IMGUI_ENUM(ImGuiDragDropFlags_SourceNoHoldToOpenOthers);
		REGISTER_IMGUI_ENUM(ImGuiDragDropFlags_SourceAllowNullID);
		REGISTER_IMGUI_ENUM(ImGuiDragDropFlags_SourceExtern);
		REGISTER_IMGUI_ENUM(ImGuiDragDropFlags_SourceAutoExpirePayload);
		REGISTER_IMGUI_ENUM(ImGuiDragDropFlags_AcceptBeforeDelivery);
		REGISTER_IMGUI_ENUM(ImGuiDragDropFlags_AcceptNoDrawDefaultRect);
		REGISTER_IMGUI_ENUM(ImGuiDragDropFlags_AcceptNoPreviewTooltip);
		REGISTER_IMGUI_ENUM(ImGuiDragDropFlags_AcceptPeekOnly);
	}
	
	{	// A primary data type
		REGISTER_IMGUI_ENUM(ImGuiDataType_S8);
		REGISTER_IMGUI_ENUM(ImGuiDataType_U8);
		REGISTER_IMGUI_ENUM(ImGuiDataType_S16);
		REGISTER_IMGUI_ENUM(ImGuiDataType_U16);
		REGISTER_IMGUI_ENUM(ImGuiDataType_S32);
		REGISTER_IMGUI_ENUM(ImGuiDataType_U32);
		REGISTER_IMGUI_ENUM(ImGuiDataType_S64);
		REGISTER_IMGUI_ENUM(ImGuiDataType_U64);
		REGISTER_IMGUI_ENUM(ImGuiDataType_Float);
		REGISTER_IMGUI_ENUM(ImGuiDataType_Double);
		REGISTER_IMGUI_ENUM(ImGuiDataType_COUNT);
	}
	
	{	// A cardinal direction
		REGISTER_IMGUI_ENUM(ImGuiDir_None);
		REGISTER_IMGUI_ENUM(ImGuiDir_Left);
		REGISTER_IMGUI_ENUM(ImGuiDir_Right);
		REGISTER_IMGUI_ENUM(ImGuiDir_Up);
		REGISTER_IMGUI_ENUM(ImGuiDir_Down);
		REGISTER_IMGUI_ENUM(ImGuiDir_COUNT);
	}
	
	{	// A sorting direction
		//REGISTER_IMGUI_ENUM(ImGuiSortDirection_None);
		//REGISTER_IMGUI_ENUM(ImGuiSortDirection_Ascending);
		//REGISTER_IMGUI_ENUM(ImGuiSortDirection_Descending);
	}
	
	{	// Keys value 0 to 511 are left unused as legacy native/opaque key values (< 1.87)
		// Keys value >= 512 are named keys (>= 1.87)
		//REGISTER_IMGUI_ENUM(ImGuiKey_None);
		REGISTER_IMGUI_ENUM(ImGuiKey_Tab);
		REGISTER_IMGUI_ENUM(ImGuiKey_LeftArrow);
		REGISTER_IMGUI_ENUM(ImGuiKey_RightArrow);
		REGISTER_IMGUI_ENUM(ImGuiKey_UpArrow);
		REGISTER_IMGUI_ENUM(ImGuiKey_DownArrow);
		REGISTER_IMGUI_ENUM(ImGuiKey_PageUp);
		REGISTER_IMGUI_ENUM(ImGuiKey_PageDown);
		REGISTER_IMGUI_ENUM(ImGuiKey_Home);
		REGISTER_IMGUI_ENUM(ImGuiKey_Insert);
		REGISTER_IMGUI_ENUM(ImGuiKey_Delete);
		REGISTER_IMGUI_ENUM(ImGuiKey_Backspace);
		REGISTER_IMGUI_ENUM(ImGuiKey_Space);
		REGISTER_IMGUI_ENUM(ImGuiKey_Enter);
		REGISTER_IMGUI_ENUM(ImGuiKey_Escape);
		//REGISTER_IMGUI_ENUM(ImGuiKey_LeftCtrl);
		//REGISTER_IMGUI_ENUM(ImGuiKey_LeftShift);
		//REGISTER_IMGUI_ENUM(ImGuiKey_LeftAlt);
		//REGISTER_IMGUI_ENUM(ImGuiKey_LeftSuper);
		//REGISTER_IMGUI_ENUM(ImGuiKey_RightCtrl);
		//REGISTER_IMGUI_ENUM(ImGuiKey_RightShift);
		//REGISTER_IMGUI_ENUM(ImGuiKey_RightAlt);
		//REGISTER_IMGUI_ENUM(ImGuiKey_RightSuper);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Menu);
		//REGISTER_IMGUI_ENUM(ImGuiKey_0);
		//REGISTER_IMGUI_ENUM(ImGuiKey_1);
		//REGISTER_IMGUI_ENUM(ImGuiKey_2);
		//REGISTER_IMGUI_ENUM(ImGuiKey_3);
		//REGISTER_IMGUI_ENUM(ImGuiKey_4);
		//REGISTER_IMGUI_ENUM(ImGuiKey_5);
		//REGISTER_IMGUI_ENUM(ImGuiKey_6);
		//REGISTER_IMGUI_ENUM(ImGuiKey_7);
		//REGISTER_IMGUI_ENUM(ImGuiKey_8);
		//REGISTER_IMGUI_ENUM(ImGuiKey_9);
		REGISTER_IMGUI_ENUM(ImGuiKey_A);
		//REGISTER_IMGUI_ENUM(ImGuiKey_B);
		REGISTER_IMGUI_ENUM(ImGuiKey_C);
		//REGISTER_IMGUI_ENUM(ImGuiKey_D);
		//REGISTER_IMGUI_ENUM(ImGuiKey_E);
		REGISTER_IMGUI_ENUM(ImGuiKey_F);
		//REGISTER_IMGUI_ENUM(ImGuiKey_G);
		//REGISTER_IMGUI_ENUM(ImGuiKey_H);
		//REGISTER_IMGUI_ENUM(ImGuiKey_I);
		//REGISTER_IMGUI_ENUM(ImGuiKey_J);
		//REGISTER_IMGUI_ENUM(ImGuiKey_K);
		//REGISTER_IMGUI_ENUM(ImGuiKey_L);
		//REGISTER_IMGUI_ENUM(ImGuiKey_M);
		//REGISTER_IMGUI_ENUM(ImGuiKey_N);
		//REGISTER_IMGUI_ENUM(ImGuiKey_O);
		//REGISTER_IMGUI_ENUM(ImGuiKey_P);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Q);
		//REGISTER_IMGUI_ENUM(ImGuiKey_R);
		//REGISTER_IMGUI_ENUM(ImGuiKey_S);
		//REGISTER_IMGUI_ENUM(ImGuiKey_T);
		//REGISTER_IMGUI_ENUM(ImGuiKey_U);
		REGISTER_IMGUI_ENUM(ImGuiKey_V);
		//REGISTER_IMGUI_ENUM(ImGuiKey_W);
		REGISTER_IMGUI_ENUM(ImGuiKey_X);
		REGISTER_IMGUI_ENUM(ImGuiKey_Y);
		REGISTER_IMGUI_ENUM(ImGuiKey_Z);
		//REGISTER_IMGUI_ENUM(ImGuiKey_F1);
		//REGISTER_IMGUI_ENUM(ImGuiKey_F2);
		//REGISTER_IMGUI_ENUM(ImGuiKey_F3);
		//REGISTER_IMGUI_ENUM(ImGuiKey_F4);
		//REGISTER_IMGUI_ENUM(ImGuiKey_F5);
		//REGISTER_IMGUI_ENUM(ImGuiKey_F6);
		//REGISTER_IMGUI_ENUM(ImGuiKey_F7);
		//REGISTER_IMGUI_ENUM(ImGuiKey_F8);
		//REGISTER_IMGUI_ENUM(ImGuiKey_F9);
		//REGISTER_IMGUI_ENUM(ImGuiKey_F10);
		//REGISTER_IMGUI_ENUM(ImGuiKey_F11);
		//REGISTER_IMGUI_ENUM(ImGuiKey_F12);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Apostrophe);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Comma);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Minus);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Period);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Slash);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Semicolon);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Equal);
		//REGISTER_IMGUI_ENUM(ImGuiKey_LeftBracket);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Backslash);
		//REGISTER_IMGUI_ENUM(ImGuiKey_RightBracket);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GraveAccent);
		//REGISTER_IMGUI_ENUM(ImGuiKey_CapsLock);
		//REGISTER_IMGUI_ENUM(ImGuiKey_ScrollLock);
		//REGISTER_IMGUI_ENUM(ImGuiKey_NumLock);
		//REGISTER_IMGUI_ENUM(ImGuiKey_PrintScreen);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Pause);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Keypad0);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Keypad1);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Keypad2);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Keypad3);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Keypad4);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Keypad5);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Keypad6);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Keypad7);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Keypad8);
		//REGISTER_IMGUI_ENUM(ImGuiKey_Keypad9);
		//REGISTER_IMGUI_ENUM(ImGuiKey_KeypadDecimal);
		//REGISTER_IMGUI_ENUM(ImGuiKey_KeypadDivide);
		//REGISTER_IMGUI_ENUM(ImGuiKey_KeypadMultiply);
		//REGISTER_IMGUI_ENUM(ImGuiKey_KeypadSubtract);
		//REGISTER_IMGUI_ENUM(ImGuiKey_KeypadAdd);
		//REGISTER_IMGUI_ENUM(ImGuiKey_KeypadEnter);
		//REGISTER_IMGUI_ENUM(ImGuiKey_KeypadEqual);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadStart);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadBack);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadFaceLeft);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadFaceRight);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadFaceUp);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadFaceDown);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadDpadLeft);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadDpadRight);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadDpadUp);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadDpadDown);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadL1);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadR1);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadL2);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadR2);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadL3);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadR3);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadLStickLeft);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadLStickRight);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadLStickUp);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadLStickDown);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadRStickLeft);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadRStickRight);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadRStickUp);
		//REGISTER_IMGUI_ENUM(ImGuiKey_GamepadRStickDown);
		//REGISTER_IMGUI_ENUM(ImGuiKey_MouseLeft);
		//REGISTER_IMGUI_ENUM(ImGuiKey_MouseRight);
		//REGISTER_IMGUI_ENUM(ImGuiKey_MouseMiddle);
		//REGISTER_IMGUI_ENUM(ImGuiKey_MouseX1);
		//REGISTER_IMGUI_ENUM(ImGuiKey_MouseX2);
		//REGISTER_IMGUI_ENUM(ImGuiKey_MouseWheelX);
		//REGISTER_IMGUI_ENUM(ImGuiKey_MouseWheelY);
		//REGISTER_IMGUI_ENUM(ImGuiKey_ReservedForModCtrl);
		//REGISTER_IMGUI_ENUM(ImGuiKey_ReservedForModShift);
		//REGISTER_IMGUI_ENUM(ImGuiKey_ReservedForModAlt);
		//REGISTER_IMGUI_ENUM(ImGuiKey_ReservedForModSuper);
		//REGISTER_IMGUI_ENUM(ImGuiKey_COUNT);
		//REGISTER_IMGUI_ENUM(ImGuiMod_None);
		//REGISTER_IMGUI_ENUM(ImGuiMod_Ctrl);
		//REGISTER_IMGUI_ENUM(ImGuiMod_Shift);
		//REGISTER_IMGUI_ENUM(ImGuiMod_Alt);
		//REGISTER_IMGUI_ENUM(ImGuiMod_Super);
		//REGISTER_IMGUI_ENUM(ImGuiMod_Mask_);
		//REGISTER_IMGUI_ENUM(ImGuiMod_Shortcut);
		//REGISTER_IMGUI_ENUM(ImGuiKey_NamedKey_BEGIN);
		//REGISTER_IMGUI_ENUM(ImGuiKey_NamedKey_END);
		//REGISTER_IMGUI_ENUM(ImGuiKey_NamedKey_COUNT);
		//REGISTER_IMGUI_ENUM(ImGuiKey_KeysData_SIZE);
		//REGISTER_IMGUI_ENUM(ImGuiKey_KeysData_OFFSET);
		//REGISTER_IMGUI_ENUM(ImGuiKey_ModCtrl);
		//REGISTER_IMGUI_ENUM(ImGuiKey_KeyPadEnter);
	}
	
	{	// OBSOLETED in 1.88 (from July 2022): ImGuiNavInput and io.NavInputs[].
		REGISTER_IMGUI_ENUM(ImGuiNavInput_Activate);
		REGISTER_IMGUI_ENUM(ImGuiNavInput_Cancel);
		REGISTER_IMGUI_ENUM(ImGuiNavInput_Input);
		REGISTER_IMGUI_ENUM(ImGuiNavInput_Menu);
		REGISTER_IMGUI_ENUM(ImGuiNavInput_DpadLeft);
		REGISTER_IMGUI_ENUM(ImGuiNavInput_DpadRight);
		REGISTER_IMGUI_ENUM(ImGuiNavInput_DpadUp);
		REGISTER_IMGUI_ENUM(ImGuiNavInput_DpadDown);
		REGISTER_IMGUI_ENUM(ImGuiNavInput_LStickLeft);
		REGISTER_IMGUI_ENUM(ImGuiNavInput_LStickRight);
		REGISTER_IMGUI_ENUM(ImGuiNavInput_LStickUp);
		REGISTER_IMGUI_ENUM(ImGuiNavInput_LStickDown);
		REGISTER_IMGUI_ENUM(ImGuiNavInput_FocusPrev);
		REGISTER_IMGUI_ENUM(ImGuiNavInput_FocusNext);
		REGISTER_IMGUI_ENUM(ImGuiNavInput_TweakSlow);
		REGISTER_IMGUI_ENUM(ImGuiNavInput_TweakFast);
		REGISTER_IMGUI_ENUM(ImGuiNavInput_COUNT);
	}
	
	{	// Configuration flags stored in io.ConfigFlags. Set by user/application.
		REGISTER_IMGUI_ENUM(ImGuiConfigFlags_None);
		REGISTER_IMGUI_ENUM(ImGuiConfigFlags_NavEnableKeyboard);
		REGISTER_IMGUI_ENUM(ImGuiConfigFlags_NavEnableGamepad);
		REGISTER_IMGUI_ENUM(ImGuiConfigFlags_NavEnableSetMousePos);
		REGISTER_IMGUI_ENUM(ImGuiConfigFlags_NavNoCaptureKeyboard);
		REGISTER_IMGUI_ENUM(ImGuiConfigFlags_NoMouse);
		REGISTER_IMGUI_ENUM(ImGuiConfigFlags_NoMouseCursorChange);
		REGISTER_IMGUI_ENUM(ImGuiConfigFlags_DockingEnable);
		REGISTER_IMGUI_ENUM(ImGuiConfigFlags_ViewportsEnable);
		REGISTER_IMGUI_ENUM(ImGuiConfigFlags_DpiEnableScaleViewports);
		REGISTER_IMGUI_ENUM(ImGuiConfigFlags_DpiEnableScaleFonts);
		REGISTER_IMGUI_ENUM(ImGuiConfigFlags_IsSRGB);
		REGISTER_IMGUI_ENUM(ImGuiConfigFlags_IsTouchScreen);
	}
	
	{	// Backend capabilities flags stored in io.BackendFlags. Set by imgui_impl_xxx or custom backend.
		REGISTER_IMGUI_ENUM(ImGuiBackendFlags_None);
		REGISTER_IMGUI_ENUM(ImGuiBackendFlags_HasGamepad);
		REGISTER_IMGUI_ENUM(ImGuiBackendFlags_HasMouseCursors);
		REGISTER_IMGUI_ENUM(ImGuiBackendFlags_HasSetMousePos);
		REGISTER_IMGUI_ENUM(ImGuiBackendFlags_RendererHasVtxOffset);
		REGISTER_IMGUI_ENUM(ImGuiBackendFlags_PlatformHasViewports);
		REGISTER_IMGUI_ENUM(ImGuiBackendFlags_HasMouseHoveredViewport);
		REGISTER_IMGUI_ENUM(ImGuiBackendFlags_RendererHasViewports);
	}
	
	{	// Enumeration for PushStyleColor() / PopStyleColor()
		REGISTER_IMGUI_ENUM(ImGuiCol_Text);
		REGISTER_IMGUI_ENUM(ImGuiCol_TextDisabled);
		REGISTER_IMGUI_ENUM(ImGuiCol_WindowBg);
		REGISTER_IMGUI_ENUM(ImGuiCol_ChildBg);
		REGISTER_IMGUI_ENUM(ImGuiCol_PopupBg);
		REGISTER_IMGUI_ENUM(ImGuiCol_Border);
		REGISTER_IMGUI_ENUM(ImGuiCol_BorderShadow);
		REGISTER_IMGUI_ENUM(ImGuiCol_FrameBg);
		REGISTER_IMGUI_ENUM(ImGuiCol_FrameBgHovered);
		REGISTER_IMGUI_ENUM(ImGuiCol_FrameBgActive);
		REGISTER_IMGUI_ENUM(ImGuiCol_TitleBg);
		REGISTER_IMGUI_ENUM(ImGuiCol_TitleBgActive);
		REGISTER_IMGUI_ENUM(ImGuiCol_TitleBgCollapsed);
		REGISTER_IMGUI_ENUM(ImGuiCol_MenuBarBg);
		REGISTER_IMGUI_ENUM(ImGuiCol_ScrollbarBg);
		REGISTER_IMGUI_ENUM(ImGuiCol_ScrollbarGrab);
		REGISTER_IMGUI_ENUM(ImGuiCol_ScrollbarGrabHovered);
		REGISTER_IMGUI_ENUM(ImGuiCol_ScrollbarGrabActive);
		REGISTER_IMGUI_ENUM(ImGuiCol_CheckMark);
		REGISTER_IMGUI_ENUM(ImGuiCol_SliderGrab);
		REGISTER_IMGUI_ENUM(ImGuiCol_SliderGrabActive);
		REGISTER_IMGUI_ENUM(ImGuiCol_Button);
		REGISTER_IMGUI_ENUM(ImGuiCol_ButtonHovered);
		REGISTER_IMGUI_ENUM(ImGuiCol_ButtonActive);
		REGISTER_IMGUI_ENUM(ImGuiCol_Header);
		REGISTER_IMGUI_ENUM(ImGuiCol_HeaderHovered);
		REGISTER_IMGUI_ENUM(ImGuiCol_HeaderActive);
		REGISTER_IMGUI_ENUM(ImGuiCol_Separator);
		REGISTER_IMGUI_ENUM(ImGuiCol_SeparatorHovered);
		REGISTER_IMGUI_ENUM(ImGuiCol_SeparatorActive);
		REGISTER_IMGUI_ENUM(ImGuiCol_ResizeGrip);
		REGISTER_IMGUI_ENUM(ImGuiCol_ResizeGripHovered);
		REGISTER_IMGUI_ENUM(ImGuiCol_ResizeGripActive);
		REGISTER_IMGUI_ENUM(ImGuiCol_Tab);
		REGISTER_IMGUI_ENUM(ImGuiCol_TabHovered);
		REGISTER_IMGUI_ENUM(ImGuiCol_TabActive);
		REGISTER_IMGUI_ENUM(ImGuiCol_TabUnfocused);
		REGISTER_IMGUI_ENUM(ImGuiCol_TabUnfocusedActive);
		REGISTER_IMGUI_ENUM(ImGuiCol_DockingPreview);
		REGISTER_IMGUI_ENUM(ImGuiCol_DockingEmptyBg);
		REGISTER_IMGUI_ENUM(ImGuiCol_PlotLines);
		REGISTER_IMGUI_ENUM(ImGuiCol_PlotLinesHovered);
		REGISTER_IMGUI_ENUM(ImGuiCol_PlotHistogram);
		REGISTER_IMGUI_ENUM(ImGuiCol_PlotHistogramHovered);
		//REGISTER_IMGUI_ENUM(ImGuiCol_TableHeaderBg);
		//REGISTER_IMGUI_ENUM(ImGuiCol_TableBorderStrong);
		//REGISTER_IMGUI_ENUM(ImGuiCol_TableBorderLight);
		//REGISTER_IMGUI_ENUM(ImGuiCol_TableRowBg);
		//REGISTER_IMGUI_ENUM(ImGuiCol_TableRowBgAlt);
		REGISTER_IMGUI_ENUM(ImGuiCol_TextSelectedBg);
		REGISTER_IMGUI_ENUM(ImGuiCol_DragDropTarget);
		REGISTER_IMGUI_ENUM(ImGuiCol_NavHighlight);
		REGISTER_IMGUI_ENUM(ImGuiCol_NavWindowingHighlight);
		REGISTER_IMGUI_ENUM(ImGuiCol_NavWindowingDimBg);
		REGISTER_IMGUI_ENUM(ImGuiCol_ModalWindowDimBg);
		REGISTER_IMGUI_ENUM(ImGuiCol_COUNT);
	}
	
	{	// PushStyleVar() / PopStyleVar()
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_Alpha);
		//REGISTER_IMGUI_ENUM(ImGuiStyleVar_DisabledAlpha);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_WindowPadding);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_WindowRounding);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_WindowBorderSize);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_WindowMinSize);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_WindowTitleAlign);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_ChildRounding);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_ChildBorderSize);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_PopupRounding);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_PopupBorderSize);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_FramePadding);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_FrameRounding);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_FrameBorderSize);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_ItemSpacing);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_ItemInnerSpacing);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_IndentSpacing);
		//REGISTER_IMGUI_ENUM(ImGuiStyleVar_CellPadding);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_ScrollbarSize);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_ScrollbarRounding);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_GrabMinSize);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_GrabRounding);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_TabRounding);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_ButtonTextAlign);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_SelectableTextAlign);
		REGISTER_IMGUI_ENUM(ImGuiStyleVar_COUNT);
	}
	
	{	// Flags for InvisibleButton() [extended in imgui_internal.h]
		REGISTER_IMGUI_ENUM(ImGuiButtonFlags_None);
		//REGISTER_IMGUI_ENUM(ImGuiButtonFlags_MouseButtonLeft);
		//REGISTER_IMGUI_ENUM(ImGuiButtonFlags_MouseButtonRight);
		//REGISTER_IMGUI_ENUM(ImGuiButtonFlags_MouseButtonMiddle);
		//REGISTER_IMGUI_ENUM(ImGuiButtonFlags_MouseButtonMask_);
		//REGISTER_IMGUI_ENUM(ImGuiButtonFlags_MouseButtonDefault_);
	}
	
	{	// Flags for ColorEdit3() / ColorEdit4() / ColorPicker3() / ColorPicker4() / ColorButton()
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_None);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_NoAlpha);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_NoPicker);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_NoOptions);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_NoSmallPreview);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_NoInputs);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_NoTooltip);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_NoLabel);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_NoSidePreview);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_NoDragDrop);
		//REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_NoBorder);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_AlphaBar);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_AlphaPreview);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_AlphaPreviewHalf);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_HDR);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_DisplayRGB);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_DisplayHSV);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_DisplayHex);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_Uint8);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_Float);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_PickerHueBar);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_PickerHueWheel);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_InputRGB);
		REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_InputHSV);
		//REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_DefaultOptions_);
		//REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_DisplayMask_);
		//REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_DataTypeMask_);
		//REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_PickerMask_);
		//REGISTER_IMGUI_ENUM(ImGuiColorEditFlags_InputMask_);
	}
	
	{	// Flags for DragFloat(), DragInt(), SliderFloat(), SliderInt() etc.
		REGISTER_IMGUI_ENUM(ImGuiSliderFlags_None);
		//REGISTER_IMGUI_ENUM(ImGuiSliderFlags_AlwaysClamp);
		//REGISTER_IMGUI_ENUM(ImGuiSliderFlags_Logarithmic);
		//REGISTER_IMGUI_ENUM(ImGuiSliderFlags_NoRoundToFormat);
		//REGISTER_IMGUI_ENUM(ImGuiSliderFlags_NoInput);
		//REGISTER_IMGUI_ENUM(ImGuiSliderFlags_InvalidMask_);
		//REGISTER_IMGUI_ENUM(ImGuiSliderFlags_ClampOnInput);
	}
	
	{	// Identify a mouse button.
		//REGISTER_IMGUI_ENUM(ImGuiMouseButton_Left);
		//REGISTER_IMGUI_ENUM(ImGuiMouseButton_Right);
		//REGISTER_IMGUI_ENUM(ImGuiMouseButton_Middle);
		//REGISTER_IMGUI_ENUM(ImGuiMouseButton_COUNT);
	}
	
	{	// Enumeration for GetMouseCursor()
		REGISTER_IMGUI_ENUM(ImGuiMouseCursor_None);
		REGISTER_IMGUI_ENUM(ImGuiMouseCursor_Arrow);
		REGISTER_IMGUI_ENUM(ImGuiMouseCursor_TextInput);
		REGISTER_IMGUI_ENUM(ImGuiMouseCursor_ResizeAll);
		REGISTER_IMGUI_ENUM(ImGuiMouseCursor_ResizeNS);
		REGISTER_IMGUI_ENUM(ImGuiMouseCursor_ResizeEW);
		REGISTER_IMGUI_ENUM(ImGuiMouseCursor_ResizeNESW);
		REGISTER_IMGUI_ENUM(ImGuiMouseCursor_ResizeNWSE);
		REGISTER_IMGUI_ENUM(ImGuiMouseCursor_Hand);
		//REGISTER_IMGUI_ENUM(ImGuiMouseCursor_NotAllowed);
		REGISTER_IMGUI_ENUM(ImGuiMouseCursor_COUNT);
	}
	
	{	// Enumeration for ImGui::SetWindow***(), SetNextWindow***(), SetNextItem***() functions
		//REGISTER_IMGUI_ENUM(ImGuiCond_None);
		REGISTER_IMGUI_ENUM(ImGuiCond_Always);
		REGISTER_IMGUI_ENUM(ImGuiCond_Once);
		REGISTER_IMGUI_ENUM(ImGuiCond_FirstUseEver);
		REGISTER_IMGUI_ENUM(ImGuiCond_Appearing);
	}
	
	{	// Flags for ImDrawList instance.
		REGISTER_IMGUI_ENUM(ImDrawListFlags_None);
		REGISTER_IMGUI_ENUM(ImDrawListFlags_AntiAliasedLines);
		//REGISTER_IMGUI_ENUM(ImDrawListFlags_AntiAliasedLinesUseTex);
		REGISTER_IMGUI_ENUM(ImDrawListFlags_AntiAliasedFill);
		REGISTER_IMGUI_ENUM(ImDrawListFlags_AllowVtxOffset);
	}
	
	{	
		REGISTER_IMGUI_ENUM(ImFontAtlasFlags_None);
		REGISTER_IMGUI_ENUM(ImFontAtlasFlags_NoPowerOfTwoHeight);
		REGISTER_IMGUI_ENUM(ImFontAtlasFlags_NoMouseCursors);
		//REGISTER_IMGUI_ENUM(ImFontAtlasFlags_NoBakedLines);
	}
	
	{	
		REGISTER_IMGUI_ENUM(ImGuiViewportFlags_None);
		//REGISTER_IMGUI_ENUM(ImGuiViewportFlags_IsPlatformWindow);
		//REGISTER_IMGUI_ENUM(ImGuiViewportFlags_IsPlatformMonitor);
		//REGISTER_IMGUI_ENUM(ImGuiViewportFlags_OwnedByApp);
		REGISTER_IMGUI_ENUM(ImGuiViewportFlags_NoDecoration);
		REGISTER_IMGUI_ENUM(ImGuiViewportFlags_NoTaskBarIcon);
		REGISTER_IMGUI_ENUM(ImGuiViewportFlags_NoFocusOnAppearing);
		REGISTER_IMGUI_ENUM(ImGuiViewportFlags_NoFocusOnClick);
		REGISTER_IMGUI_ENUM(ImGuiViewportFlags_NoInputs);
		REGISTER_IMGUI_ENUM(ImGuiViewportFlags_NoRendererClear);
		REGISTER_IMGUI_ENUM(ImGuiViewportFlags_TopMost);
		REGISTER_IMGUI_ENUM(ImGuiViewportFlags_Minimized);
		//REGISTER_IMGUI_ENUM(ImGuiViewportFlags_NoAutoMerge);
		REGISTER_IMGUI_ENUM(ImGuiViewportFlags_CanHostOtherWindows);
	}
	
	{	// flags for AddRect(), AddRectFilled(), AddImageRounded().
		//REGISTER_IMGUI_ENUM(ImDrawCornerFlags_None);
		REGISTER_IMGUI_ENUM(ImDrawCornerFlags_TopLeft);
		REGISTER_IMGUI_ENUM(ImDrawCornerFlags_TopRight);
		REGISTER_IMGUI_ENUM(ImDrawCornerFlags_BotLeft);
		REGISTER_IMGUI_ENUM(ImDrawCornerFlags_BotRight);
		REGISTER_IMGUI_ENUM(ImDrawCornerFlags_All);
		REGISTER_IMGUI_ENUM(ImDrawCornerFlags_Top);
		REGISTER_IMGUI_ENUM(ImDrawCornerFlags_Bot);
		REGISTER_IMGUI_ENUM(ImDrawCornerFlags_Left);
		REGISTER_IMGUI_ENUM(ImDrawCornerFlags_Right);
	}

	#undef REGISTER_IMGUI_ENUM
}

static void Register_ImGuiStyleAdapter(sol::table& ns_table)
{
	// ImGuiStyle
	ns_table.new_usertype<ImGuiStyle>("ImGuiStyle", sol::call_constructor, sol::constructors<
		sol::types<>>(),
		REGISTER_MEMBER_FUNC_ImGuiStyle(WindowRounding),
		REGISTER_MEMBER_FUNC_ImGuiStyle(WindowBorderSize),
		REGISTER_MEMBER_FUNC_ImGuiStyle(ItemSpacing),
		REGISTER_MEMBER_FUNC_ImGuiStyle(Alpha),
		"GetColor", [](ImGuiStyle& adapter, int index)->ImVec4 {
			ImVec4 c(0.0f, 0.0f, 0.0f, 0.0f);
			if (index >= 0
				&& index < ImGuiCol_COUNT)
			{
				c = adapter.Colors[index];
			}
			return c;
		}
		);
}

static void Register_ImGuiViewportAdapter(sol::table& ns_table)
{
	// ImGuiViewport
	ns_table.new_usertype<ImGuiViewport>("ImGuiViewport", sol::call_constructor, sol::constructors<
		sol::types<>>(),
		REGISTER_MEMBER_FUNC_ImGuiViewport(ID),
		REGISTER_MEMBER_FUNC_ImGuiViewport(Pos),
		REGISTER_MEMBER_FUNC_ImGuiViewport(Size)
	);
}

static void Register_ImGuiPayloadAdapter(sol::table& ns_table)
{
	// ImGuiStyle
	ns_table.new_usertype<ImGuiPayload>("ImGuiPayload", sol::call_constructor, sol::constructors<
		sol::types<>>(),
		"GetData", [](ImGuiPayload& adapter, int index)->std::string {
			char* buf = static_cast<char*>(adapter.Data);
			int size = adapter.DataSize;
			return std::string(buf, size);
		},
		REGISTER_MEMBER_FUNC_ImGuiPayload(IsDataType),
		REGISTER_MEMBER_FUNC_ImGuiPayload(IsPreview),
		REGISTER_MEMBER_FUNC_ImGuiPayload(IsDelivery)
	);
}

void Bind_Imgui(sol::state& sol_state)
{
	sol::table ns_table = sol_state["imguifunction"].get_or_create<sol::table>();

	Register_ImGui(ns_table);
	Register_ImGuiStyleAdapter(ns_table);
	Register_ImGuiViewportAdapter(ns_table);
	Register_ImGuiPayloadAdapter(ns_table);
}

NS_JYE_END

#endif

