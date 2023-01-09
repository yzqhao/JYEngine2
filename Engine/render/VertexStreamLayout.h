#pragma once

#include "Engine/private/Define.h"
#include "RHI/RHIDefine.h"
#include "Core/Interface/ILogSystem.h"

NS_JYE_BEGIN

class ENGINE_API VertexStreamLayout	
{
public:
#pragma pack(push,1)
	class Layout
	{
	private:
		RHIDefine::ShaderAttribute	m_eAttributes;//顶点的类型
		RHIDefine::DataType	m_eDataType;//顶点数据的类型
		bool m_isNormalize;//是否规范化的，一般为false
		byte m_uDataCount;//每个定点数据的个数
		byte m_uStride;//每个数据段的bite偏移量
		byte m_uOffset;//每个数据段的起始地址
	public:
		Layout()
			:m_eAttributes((RHIDefine::ShaderAttribute)-1),
			m_eDataType((RHIDefine::DataType)-1),
			m_isNormalize(false),
			m_uDataCount(0),
			m_uStride(0),
			m_uOffset(0)
		{
		}
		Layout(RHIDefine::ShaderAttribute attr, RHIDefine::DataType dest, byte count, byte offset)
			:m_eAttributes(attr),
			m_eDataType(dest),
			m_uDataCount(count),
			m_isNormalize(false),
			m_uStride(0),
			m_uOffset(offset)
		{
			assert(0 != count);
			switch (dest)
			{
			case RHIDefine::DT_FLOAT: m_uStride = count * sizeof(float); m_isNormalize = false; break;
			case RHIDefine::DT_HALF_FLOAT: m_uStride = count * sizeof(short); m_isNormalize = false; break;
			case RHIDefine::DT_INT_8_8_8_8: m_uStride = sizeof(int); m_isNormalize = true; break;
			case RHIDefine::DT_UINT_8_8_8_8: m_uStride = sizeof(int); m_isNormalize = true; break;
			case RHIDefine::DT_INT_2_10_10_10: m_uStride = sizeof(int); m_isNormalize = true; break;
			case RHIDefine::DT_UINT_10_10_10_2: m_uStride = sizeof(int); m_isNormalize = true; break;
			default: JYLOG("Unkown vertex data type !");
			}
		}
	public:
		inline bool isActive()
		{
			return 0 != m_uDataCount;
		}
		inline RHIDefine::ShaderAttribute GetAttributes() const
		{
			return m_eAttributes;
		}
		inline byte DataCount() const
		{
			return m_uDataCount;
		}
		inline RHIDefine::DataType DataType() const
		{
			return m_eDataType;
		}
		inline bool isNormalize() const
		{
			return m_isNormalize;
		}
		inline byte Stride() const
		{
			return m_uStride;
		}
		inline byte ByteSize() const//当前layout的顶点数据byte大小值
		{
			return m_uStride;
		}
		inline byte Offset() const
		{
			return m_uOffset;
		}
	};
#pragma pack(pop)
private:
	typedef Vector<Layout*>	LayoutVector;
	typedef std::map<RHIDefine::ShaderAttribute, int>	IndexMapping;
private:
	LayoutVector		m_LayoutArray;
	IndexMapping		m_IndexMapping;
	int					m_uStride;
private:
	inline int _GetAttributeIndex(RHIDefine::ShaderAttribute att)
	{
		IndexMapping::iterator it = m_IndexMapping.find(att);
		if (m_IndexMapping.end() == it)
		{
			int index = m_LayoutArray.size();
			m_LayoutArray.push_back(NULL);
			m_IndexMapping.insert({ att, index });
			return index;
		}
		return it->second;
	}
	inline void _Copy(const VertexStreamLayout& rhs)
	{
		m_uStride = rhs.m_uStride;
		m_IndexMapping = rhs.m_IndexMapping;
		for (auto layout : rhs.m_LayoutArray)
		{
			m_LayoutArray.push_back(new Layout(*layout));
		}
	}
public:
	VertexStreamLayout(void)
		:m_uStride(0)
	{
	}
	VertexStreamLayout(const VertexStreamLayout& rhs)
	{
		_Copy(rhs);
	}
	~VertexStreamLayout(void)
	{
		for (auto layout : m_LayoutArray)
		{
			delete(layout);
		}
	}
	const VertexStreamLayout& operator=(const VertexStreamLayout& rhs)
	{
		Clear();
		_Copy(rhs);
		return *this;
	}
public:
	inline void Clear()
	{
		m_uStride = 0;
		for (auto layout : m_LayoutArray)
		{
			delete(layout);
		}
		m_IndexMapping.clear();
		m_LayoutArray.clear();
	}
	inline void SetVertexLayout(RHIDefine::ShaderAttribute attr, RHIDefine::DataType dest, byte size)
	{
		int index = _GetAttributeIndex(attr);
		if (NULL != m_LayoutArray[index])
		{
			JYLOG("vertex layout already exist");
		}
		else
		{
			m_LayoutArray[index] = new Layout(attr, dest, size, m_uStride);
			m_uStride += m_LayoutArray[index]->Stride();
		}
	}
	inline const Layout* GetVertexLayout(RHIDefine::ShaderAttribute attr) const
	{
		IndexMapping::const_iterator it = m_IndexMapping.find(attr);
		return m_IndexMapping.end() != it
			? m_LayoutArray[it->second]
			: NULL;
	}
	inline const Vector<VertexStreamLayout::Layout*> GetVertexLayouts() const
	{
		Vector<VertexStreamLayout::Layout*> layouts;
		auto iter = m_IndexMapping.begin();
		while (iter != m_IndexMapping.end())
		{
			layouts.push_back(m_LayoutArray[iter->second]);
			iter++;
		}
		return layouts;
	}
	inline int GetStride() const { return m_uStride; }
};

typedef List<VertexStreamLayout::Layout> LayoutList;

NS_JYE_END