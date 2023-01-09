#include "SubMesh.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI(SubMesh, Object);
BEGIN_ADD_PROPERTY(SubMesh, Object);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(SubMesh)
IMPLEMENT_INITIAL_END

SubMesh::SubMesh()
	: m_isEnable(false)
	, m_hashValue(0)
{

}

SubMesh::SubMesh(const SubMesh& rhs)
	: m_isEnable(rhs.m_isEnable)
	, m_DrawRange(rhs.m_DrawRange)
	, m_hashValue(0)
{
	ReculateHash();
}

SubMesh::~SubMesh()
{

}

NS_JYE_END