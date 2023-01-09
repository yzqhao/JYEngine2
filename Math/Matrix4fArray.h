#pragma once

#include "Mat4.h"

#include <vector>

NS_JYE_MATH_BEGIN

template<int dim>
class MatrixfArray
{
private:
	enum Constant
	{
		VAC_DATA_COUNT = dim * dim,
	};
	std::vector<float> m_Data;
	bool m_Transpose = false;
public:
	inline bool operator == (const MatrixfArray& rhs)
	{
		return m_Data == rhs.m_Data;
	}
	inline bool operator != (const MatrixfArray& rhs)
	{
		return m_Data != rhs.m_Data;
	}
	inline void Set(int index, const Mat4& data)
	{
		memcpy(&m_Data.at(index * VAC_DATA_COUNT), data.GetPrt(), sizeof(float) * VAC_DATA_COUNT);
	}
	inline void Resize(int size)
	{
		m_Data.resize(size * VAC_DATA_COUNT);
	}
	inline void PushBack(const Mat4& data)
	{
		memcpy(m_Data.data() + m_Data.size() * VAC_DATA_COUNT, data.GetPrt(), sizeof(data));
	}
	inline void Clear()
	{
		m_Data.clear();
	}
	inline uint Size() const
	{
		return m_Data.size() / VAC_DATA_COUNT;
	}
	inline float* Data()
	{
		return m_Data.data();
	}
	inline const float* Data() const
	{
		return m_Data.data();
	}
	inline bool Transpose()
	{
		return m_Transpose;
	}
	inline void setTranspose(bool transpose)
	{
		m_Transpose = transpose;
	}
};
typedef MatrixfArray<3> Matrix3fArray;
typedef MatrixfArray<4> Matrix4fArray;

NS_JYE_MATH_END