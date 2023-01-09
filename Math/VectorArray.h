#pragma once

#include "Math.h"

NS_JYE_MATH_BEGIN

template<typename T, size_t VAC_DATA_COUNT, typename Container = std::vector< T >>
class DataArray {
protected:
	Container m_Data;
public:
	inline bool operator == (const DataArray& rhs)
	{
		return m_Data == rhs.m_Data;
	}
	inline bool operator != (const DataArray& rhs)
	{
		return m_Data != rhs.m_Data;
	}

	inline uint Size() const
	{
		return m_Data.size() / VAC_DATA_COUNT;
	}
	inline void Resize(int size)
	{
		m_Data.resize(size * VAC_DATA_COUNT);
	}
	inline void Clear()
	{
		m_Data.clear();
	}
	inline T* Data()
	{
		return m_Data.data();
	}
	inline const T* Data() const
	{
		return m_Data.data();
	}
};


template<typename T, size_t VAC_DATA_COUNT = 1>
class VectorArray : public DataArray<T, VAC_DATA_COUNT>
{
public:
	VectorArray() = default;
	VectorArray(T* data, size_t size) {
		this->Fill(data, size);
	}
};
using VectorArrayF = VectorArray<float>;
using VectorArrayI = VectorArray<int>;


template<typename T, size_t VAC_DATA_COUNT = 2>
class Vector2Array : public DataArray<T, VAC_DATA_COUNT>
{
public:
	inline void Set(int index, const Vec2& data)
	{
		memcpy(&this->m_Data.at(index * VAC_DATA_COUNT), data.GetPrt(), sizeof(T) * VAC_DATA_COUNT);
	}
	inline void PushBack(const Vec2& data)
	{
		this->m_Data.push_back(data.x);
		this->m_Data.push_back(data.y);
	}
	Vector2Array() = default;
};
using Vector2ArrayF = Vector2Array<float>;
using Vector2ArrayI = Vector2Array<int>;


template<typename T, size_t VAC_DATA_COUNT = 3>
class Vector3Array : public DataArray<T, VAC_DATA_COUNT>
{
public:
	inline void Set(int index, const Vec3& data)
	{
		memcpy(&this->m_Data.at(index * VAC_DATA_COUNT), data.GetPrt(), sizeof(T) * VAC_DATA_COUNT);
	}
	inline void PushBack(const Vec3& data)
	{
		this->m_Data.push_back(data.x);
		this->m_Data.push_back(data.y);
		this->m_Data.push_back(data.z);
	}
};
using Vector3ArrayF = Vector3Array<float>;
using Vector3ArrayI = Vector3Array<int>;


template<typename T, size_t VAC_DATA_COUNT = 4>
class Vector4Array : public DataArray<T, VAC_DATA_COUNT>
{
public:
	inline void Set(int index, const Vec4& data)
	{
		memcpy(&this->m_Data.at(index * VAC_DATA_COUNT), data.GetPrt(), sizeof(T) * VAC_DATA_COUNT);
	}
	inline void PushBack(const Vec4& data)
	{
		this->m_Data.push_back(data.x);
		this->m_Data.push_back(data.y);
		this->m_Data.push_back(data.z);
		this->m_Data.push_back(data.w);
	}
};
using Vector4ArrayF = Vector4Array<float>;
using Vector4ArrayI = Vector4Array<int>;

NS_JYE_MATH_END
