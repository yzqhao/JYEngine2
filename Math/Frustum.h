
#pragma once

#include "Plane.h"
#include "Mat4.h"

NS_JYE_MATH_BEGIN

class MATH_API Frustum 
{
public:
	enum PalneName
	{
		FRUSTUM_PLANE_LEFT = 0,
		FRUSTUM_PLANE_RIGHT = 1,
		FRUSTUM_PLANE_BOTTOM = 2,
		FRUSTUM_PLANE_TOP = 3,
		FRUSTUM_PLANE_NEAR = 4,
		FRUSTUM_PLANE_FAR = 5,
		FACE_COUNT = 6,
	};
	enum CornerName
	{
		FRUSTUM_CORNER_LBN = 0, // left bottom near
		FRUSTUM_CORNER_RBN = 1, // right bottom near
		FRUSTUM_CORNER_LTN = 2, // left top near
		FRUSTUM_CORNER_RTN = 3, // right top near
		FRUSTUM_CORNER_LBF = 4, // left bottom far
		FRUSTUM_CORNER_RBF = 5, // right bottom far
		FRUSTUM_CORNER_LTF = 6, // left top far
		FRUSTUM_CORNER_RTF = 7, // right top far

		FRUSTUM_CORNER_COUNT = 8,
	};
private:
	Plane m_Planes[FACE_COUNT];
	Vec3 m_Corners[FRUSTUM_CORNER_COUNT];
	mutable float m_Data[3 * FRUSTUM_CORNER_COUNT];
	int m_VertexLUT[FACE_COUNT];

public:
	FORCEINLINE int GetPlaneLUT(PalneName pn) const
	{
		return m_VertexLUT[static_cast<int>(pn)];
	}
	FORCEINLINE const Plane& GetPlane(PalneName pn) const
	{
		return m_Planes[static_cast<int>(pn)];
	}
	FORCEINLINE const Vec3& GetCorner(CornerName cn) const
	{
		return m_Corners[static_cast<int>(cn)];
	}
	FORCEINLINE void FromMatrix(const Mat4& clip, const Mat4& inv_clip)
	{
		m_Corners[0] = Vec3(-1, -1, -1) * inv_clip; // left bottom near
		m_Corners[1] = Vec3(+1, -1, -1) * inv_clip; // right bottom near
		m_Corners[2] = Vec3(-1, +1, -1) * inv_clip; // left top near
		m_Corners[3] = Vec3(+1, +1, -1) * inv_clip; // right top near
		m_Corners[4] = Vec3(-1, -1, 1) * inv_clip; // left bottom far
		m_Corners[5] = Vec3(+1, -1, 1) * inv_clip; // right bottom far
		m_Corners[6] = Vec3(-1, +1, 1) * inv_clip; // left top far
		m_Corners[7] = Vec3(+1, +1, 1) * inv_clip; // right top far


		//http://gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf

		Vec4 column1(clip.a11, clip.a21, clip.a31, clip.a41);
		Vec4 column2(clip.a12, clip.a22, clip.a32, clip.a42);
		Vec4 column3(clip.a13, clip.a23, clip.a33, clip.a43);
		Vec4 column4(clip.a14, clip.a24, clip.a34, clip.a44);

		m_Planes[FRUSTUM_PLANE_LEFT] = column4 + column1;  // left
		m_Planes[FRUSTUM_PLANE_RIGHT] = column4 - column1;  // right
		m_Planes[FRUSTUM_PLANE_BOTTOM] = column4 + column2;  // bottom
		m_Planes[FRUSTUM_PLANE_TOP] = column4 - column2;  // top
		m_Planes[FRUSTUM_PLANE_NEAR] = column4 + column3;  // near
		m_Planes[FRUSTUM_PLANE_FAR] = column4 - column3;  // far

		// Loop through each side of the frustum and normalize it.
		for (int i = 0; i < FACE_COUNT; i++)
		{
			m_Planes[i].Normalize();
		}

		//  build a bit-field that will tell us the indices for the nearest and farthest vertices from each plane...
		for (int i = 0; i < 6; ++i)
		{
			m_VertexLUT[i] = ((m_Planes[i].A() < 0) ? 1 : 0) | ((m_Planes[i].B() < 0) ? 2 : 0) | ((m_Planes[i].C() < 0) ? 4 : 0);
		}
	}

};

NS_JYE_MATH_END
