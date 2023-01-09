#pragma once

namespace ImGui
{
	// call inside your own window and before Manipulate() in order to draw gizmo to that window.
	void set_draw_list();

	// call BeginFrame right after ImGui_XXXX_NewFrame();
	void set_view_rect(float x, float y, float width, float height);

	// return true if mouse cursor is over any gizmo control (axis, plan or screen component)
	bool is_over();

	// return true if mouse IsOver or if the gizmo is in moving state
	bool is_using();

	// enable/disable the gizmo. Stay in the state until next call to Enable.
	// gizmo is rendered with gray half transparent color when disabled
	void enable(bool enable);

	// Render a cube with face color corresponding to face normal. Usefull for debug/tests
	void draw_cube(const float* view, const float* projection, float* matrix);

	// Render an ellipse
	void draw_ellipse(float* position, float rx, float ry, float r, float g, float b, float a, float *px, float *py, bool is_dashed);

	// call it when you want a gizmo
	// Needs view and projection matrices.
	// matrix parameter is the source matrix (where will be gizmo be drawn) and might be transformed by the
	// function. Return deltaMatrix is optional
	// translation is applied in world space
	enum operation
	{
		op_null,
		translate,
		rotate,
		scale
	};

	enum mode
	{
		md_null,
		local,
		world
	};

	void manipulate(const float* view, const float* projection, operation op, mode mod, float* matrix,
					float* deltaMatrix = nullptr, float* snap = nullptr, float* localBounds = nullptr,
					float* boundsSnap = nullptr, bool show2D = false);
	bool GetEndTrans();
};
