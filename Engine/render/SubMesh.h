#pragma once

#include "Engine/private/Define.h"
#include "RHI/RHIDefine.h"
#include "Core/Object.h"
#include "Math/IntVec2.h"

NS_JYE_BEGIN

class ENGINE_API SubMesh : public Object
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	Math::IntVec2	m_DrawRange;
	bool			m_isEnable;
	size_t          m_hashValue;
public:
	SubMesh();
	SubMesh(const SubMesh& rhs);
	~SubMesh();

	FORCEINLINE void Enable();
	FORCEINLINE void Disable();
	FORCEINLINE bool isEnable();
	FORCEINLINE void SetDrawRange(uint offset, uint count);
	FORCEINLINE void ReculateHash();
	FORCEINLINE size_t GetHash() const;
	FORCEINLINE const Math::IntVec2& GetDrawRange() const;
};
DECLARE_Ptr(SubMesh);
TYPE_MARCO(SubMesh);

FORCEINLINE void SubMesh::Enable()
{
	m_isEnable = true;
}

FORCEINLINE void SubMesh::Disable()
{
	m_isEnable = false;
}

FORCEINLINE bool SubMesh::isEnable()
{
	return m_isEnable;
}

FORCEINLINE void SubMesh::SetDrawRange(uint offset, uint count)
{
	m_DrawRange.x = count;
	m_DrawRange.y = offset;

	ReculateHash();
}

FORCEINLINE const Math::IntVec2& SubMesh::GetDrawRange() const
{
	return m_DrawRange;
}

FORCEINLINE void SubMesh::ReculateHash()
{
	std::hash<std::string> hashV;
	std::string strV;
	strV.append(std::to_string(m_DrawRange.x));
	strV.append(std::to_string(m_DrawRange.y));

	m_hashValue = hashV(strV);
}

FORCEINLINE size_t SubMesh::GetHash() const
{
	return m_hashValue;
}

NS_JYE_END