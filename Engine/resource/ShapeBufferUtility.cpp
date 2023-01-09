#include "ShapeBufferUtility.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI(QuadIndicesMetadata, IndicesMetadata);
BEGIN_ADD_PROPERTY(QuadIndicesMetadata, IndicesMetadata);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(QuadIndicesMetadata)
IMPLEMENT_INITIAL_END

QuadIndicesMetadata::QuadIndicesMetadata(RHIDefine::MemoryUseage mu) :IndicesMetadata(mu)
{

}

void* QuadIndicesMetadata::_ReturnMetadata()
{
	return &m_Stream;
}

void QuadIndicesMetadata::GetIdentifier(std::string& hash) const/*在str中推入表示*/
{
	hash.append(this->GetTypeName());
	if (GetMemoryUseage() == RHIDefine::MU_DYNAMIC)
	{
		hash.append("dynamic");
	}
	else
	{
		hash.append("static");
	}
}

void QuadIndicesMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
	m_Stream.SetIndicesType(RHIDefine::IT_UINT16);
	m_Stream.ReserveBuffer(6);
	m_Stream.PushIndicesDataFast(0);
	m_Stream.PushIndicesDataFast(1);
	m_Stream.PushIndicesDataFast(2);

	m_Stream.PushIndicesDataFast(2);
	m_Stream.PushIndicesDataFast(1);
	m_Stream.PushIndicesDataFast(3);
}

void QuadIndicesMetadata::ReleaseMetadate()
{
	m_Stream.Clear();
}

//
// BoxIndicesMetadata
// 
IMPLEMENT_RTTI(BoxIndicesMetadata, IndicesMetadata);
BEGIN_ADD_PROPERTY(BoxIndicesMetadata, IndicesMetadata);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(BoxIndicesMetadata)
IMPLEMENT_INITIAL_END

BoxIndicesMetadata::BoxIndicesMetadata(RHIDefine::MemoryUseage mu) :IndicesMetadata(mu)
{

}

void* BoxIndicesMetadata::_ReturnMetadata()
{
	return &m_Stream;
}

void BoxIndicesMetadata::GetIdentifier(std::string& hash) const/*?str?????*/
{
	hash.append(this->GetTypeName());
	if (GetMemoryUseage() == RHIDefine::MU_DYNAMIC)
	{
		hash.append("dynamic");
	}
	else
	{
		hash.append("static");
	}
}

void BoxIndicesMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
	m_Stream.SetIndicesType(RHIDefine::IT_UINT16);
	m_Stream.ReserveBuffer(36);
	//front
	m_Stream.PushIndicesDataFast(0);
	m_Stream.PushIndicesDataFast(2);
	m_Stream.PushIndicesDataFast(3);
	m_Stream.PushIndicesDataFast(0);
	m_Stream.PushIndicesDataFast(3);
	m_Stream.PushIndicesDataFast(1);

	//back
	m_Stream.PushIndicesDataFast(5);
	m_Stream.PushIndicesDataFast(4);
	m_Stream.PushIndicesDataFast(7);
	m_Stream.PushIndicesDataFast(5);
	m_Stream.PushIndicesDataFast(7);
	m_Stream.PushIndicesDataFast(6);

	//left
	m_Stream.PushIndicesDataFast(6);
	m_Stream.PushIndicesDataFast(7);
	m_Stream.PushIndicesDataFast(2);
	m_Stream.PushIndicesDataFast(6);
	m_Stream.PushIndicesDataFast(2);
	m_Stream.PushIndicesDataFast(0);

	//right
	m_Stream.PushIndicesDataFast(1);
	m_Stream.PushIndicesDataFast(3);
	m_Stream.PushIndicesDataFast(4);
	m_Stream.PushIndicesDataFast(1);
	m_Stream.PushIndicesDataFast(4);
	m_Stream.PushIndicesDataFast(5);

	//top
	m_Stream.PushIndicesDataFast(6);
	m_Stream.PushIndicesDataFast(0);
	m_Stream.PushIndicesDataFast(1);
	m_Stream.PushIndicesDataFast(6);
	m_Stream.PushIndicesDataFast(1);
	m_Stream.PushIndicesDataFast(5);

	//bottom
	m_Stream.PushIndicesDataFast(4);
	m_Stream.PushIndicesDataFast(3);
	m_Stream.PushIndicesDataFast(2);
	m_Stream.PushIndicesDataFast(4);
	m_Stream.PushIndicesDataFast(2);
	m_Stream.PushIndicesDataFast(7);
}

void BoxIndicesMetadata::ReleaseMetadate()
{
	m_Stream.Clear();
}

//
// BoxVertexMetadata
// 
IMPLEMENT_RTTI(BoxVertexMetadata, VertexMetadata);
BEGIN_ADD_PROPERTY(BoxVertexMetadata, VertexMetadata);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(BoxVertexMetadata)
IMPLEMENT_INITIAL_END

BoxVertexMetadata::BoxVertexMetadata(RHIDefine::MemoryUseage mu, bool hasUV, bool hasNormal)
	: VertexMetadata(mu)
	, m_hasUV(hasUV)
	, m_hasNormal(hasNormal)
{
}

void* BoxVertexMetadata::_ReturnMetadata()
{
	return &m_Stream;
}

void BoxVertexMetadata::GetIdentifier(std::string& hash) const
{
	hash.append(this->GetTypeName());
	hash.append(m_hasNormal ? "normal" : "none");
	hash.append(m_hasUV ? "uv" : "none");
	if (GetMemoryUseage() == RHIDefine::MU_DYNAMIC)
	{
		hash.append("dynamic");
	}
	else
	{
		hash.append("static");
	}
}

void BoxVertexMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
	m_Stream.ReserveBuffer(8);
	m_Stream.SetVertexType<Math::Vec4>(RHIDefine::PS_ATTRIBUTE_POSITION, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
	m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(-0.5, 0.5, 0.5, 1).GetPtr());
	m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(0.5, 0.5, 0.5, 1).GetPtr());
	m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(-0.5, -0.5, 0.5, 1).GetPtr());
	m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(0.5, -0.5, 0.5, 1).GetPtr());
	m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(0.5, -0.5, -0.5, 1).GetPtr());
	m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(0.5, 0.5, -0.5, 1).GetPtr());
	m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(-0.5, 0.5, -0.5, 1).GetPtr());
	m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(-0.5, -0.5, -0.5, 1).GetPtr());

	if (m_hasNormal)
	{
		m_Stream.SetVertexType<Math::Vec3>(RHIDefine::PS_ATTRIBUTE_NORMAL, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(0, 0, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(0, 0, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(0, 0, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(0, 0, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(0, 0, -1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(0, 0, -1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(0, 0, -1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(0, 0, -1).GetPtr());
	}

	if (m_hasUV)
	{
		m_Stream.SetVertexType<Math::Vec2>(RHIDefine::PS_ATTRIBUTE_COORDNATE0, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(0, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(1, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(0, 0).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(1, 0).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(0, 0).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(0, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(1, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(1, 0).GetPtr());
	}
}

void BoxVertexMetadata::ReleaseMetadate()
{
	m_Stream.Clear();
}

//
// SphereIndicesMetadata
//
IMPLEMENT_RTTI(SphereIndicesMetadata, VertexMetadata);
BEGIN_ADD_PROPERTY(SphereIndicesMetadata, VertexMetadata);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(SphereIndicesMetadata)
IMPLEMENT_INITIAL_END

SphereIndicesMetadata::SphereIndicesMetadata(RHIDefine::MemoryUseage mu, int sectorCount,
	int stackCount)
	: IndicesMetadata(mu)
	, m_sectorCount(sectorCount)
	, m_stackCount(stackCount)
{

}

void* SphereIndicesMetadata::_ReturnMetadata()
{
	return &m_Stream;
}

void SphereIndicesMetadata::GetIdentifier(std::string& hash) const/*?str?????*/
{
	hash.append(this->GetTypeName());
	hash.append(std::to_string(m_sectorCount));
	hash.append(std::to_string(m_stackCount));
	if (GetMemoryUseage() == RHIDefine::MU_DYNAMIC)
	{
		hash.append("dynamic");
	}
	else
	{
		hash.append("static");
	}
}

void SphereIndicesMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
	int reserved = (m_stackCount - 2) * m_sectorCount * 6 + 2 * m_sectorCount * 3;
	m_Stream.SetIndicesType(RHIDefine::IT_UINT16);
	m_Stream.ReserveBuffer(reserved);

	// indices
	//  k1--k1+1
	//  |  / |
	//  | /  |
	//  k2--k2+1
	unsigned int k1, k2;
	for (int i = 0; i < m_stackCount; ++i)
	{
		k1 = i * (m_sectorCount + 1);     // beginning of current stack
		k2 = k1 + m_sectorCount + 1;      // beginning of next stack

		for (int j = 0; j < m_sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding 1st and last stacks
			if (i != 0)
			{
				m_Stream.PushIndicesDataFast(k1);
				m_Stream.PushIndicesDataFast(k2);
				m_Stream.PushIndicesDataFast(k1 + 1);
			}

			if (i != (m_stackCount - 1))
			{
				m_Stream.PushIndicesDataFast(k1 + 1);
				m_Stream.PushIndicesDataFast(k2);
				m_Stream.PushIndicesDataFast(k2 + 1);
			}
		}
	}
}

void SphereIndicesMetadata::ReleaseMetadate()
{
	m_Stream.Clear();
}

//
// SphereVertexMetadata
//
IMPLEMENT_RTTI(SphereVertexMetadata, VertexMetadata);
BEGIN_ADD_PROPERTY(SphereVertexMetadata, VertexMetadata);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(SphereVertexMetadata)
IMPLEMENT_INITIAL_END

SphereVertexMetadata::SphereVertexMetadata(RHIDefine::MemoryUseage mu, float radius, int sectorCount, int stackCount, bool hasUV, bool hasNormal)
	: VertexMetadata(mu)
	, m_hasUV(hasUV)
	, m_hasNormal(hasNormal)
	, m_radius(radius)
	, m_sectorCount(sectorCount)
	, m_stackCount(stackCount)
{
}

void* SphereVertexMetadata::_ReturnMetadata()
{
	return &m_Stream;
}

void SphereVertexMetadata::GetIdentifier(std::string& hash) const
{
	hash.append(this->GetTypeName());
	hash.append(m_hasNormal ? "normal" : "none");
	hash.append(m_hasUV ? "uv" : "none");
	hash.append(std::to_string(m_radius));
	hash.append(std::to_string(m_sectorCount));
	hash.append(std::to_string(m_stackCount));
	if (GetMemoryUseage() == RHIDefine::MU_DYNAMIC)
	{
		hash.append("dynamic");
	}
	else
	{
		hash.append("static");
	}
}

void SphereVertexMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
	int totalVertices = (m_stackCount + 1) * (m_sectorCount + 1);

	m_Stream.ReserveBuffer(totalVertices);
	m_Stream.SetVertexType<Math::Vec4>(RHIDefine::PS_ATTRIBUTE_POSITION, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
	if (m_hasNormal)
	{
		m_Stream.SetVertexType<Math::Vec3>(RHIDefine::PS_ATTRIBUTE_NORMAL, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
	}
	if (m_hasUV)
	{
		m_Stream.SetVertexType<Math::Vec2>(RHIDefine::PS_ATTRIBUTE_COORDNATE0, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
	}

	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / m_radius;    // normal
	float s, t;                                     // texCoord

	float sectorStep = 2 * Math::PI / m_sectorCount;
	float stackStep = Math::PI / m_stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= m_stackCount; ++i)
	{
		stackAngle = Math::PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = m_radius * cosf(stackAngle);             // r * cos(u)
		z = m_radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= m_sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

													// vertex position
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(x, y, z, 1).GetPtr());

			if (m_hasNormal)
			{
				// normalized vertex normal
				nx = x * lengthInv;
				ny = y * lengthInv;
				nz = z * lengthInv;
				m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(nx, ny, nz).GetPtr());
			}

			if (m_hasUV)
			{
				// vertex tex coord between [0, 1]
				s = (float)j / m_sectorCount;
				t = (float)i / m_stackCount;
				m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(s, t).GetPtr());
			}
		}
	}
}

void SphereVertexMetadata::ReleaseMetadate()
{
	m_Stream.Clear();
}


//
// ConeIndicesMetadata
//
IMPLEMENT_RTTI(ConeIndicesMetadata, IndicesMetadata);
BEGIN_ADD_PROPERTY(ConeIndicesMetadata, IndicesMetadata);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(ConeIndicesMetadata)
IMPLEMENT_INITIAL_END

ConeIndicesMetadata::ConeIndicesMetadata(RHIDefine::MemoryUseage mu, int sectorCount)
	: IndicesMetadata(mu)
	, m_sectorCount(sectorCount)
{

}

void* ConeIndicesMetadata::_ReturnMetadata()
{
	return &m_Stream;
}

void ConeIndicesMetadata::GetIdentifier(std::string& hash) const/*?str?????*/
{
	hash.append(this->GetTypeName());
	hash.append(std::to_string(m_sectorCount));
	if (GetMemoryUseage() == RHIDefine::MU_DYNAMIC)
	{
		hash.append("dynamic");
	}
	else
	{
		hash.append("static");
	}
}

void ConeIndicesMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
	int reserved = 3 * m_sectorCount * 2;
	m_Stream.SetIndicesType(RHIDefine::IT_UINT16);
	m_Stream.ReserveBuffer(reserved);
	//side face
	for (int i = 1; i <= m_sectorCount; ++i)
	{
		m_Stream.PushIndicesDataFast(0);
		if (i == m_sectorCount)
			m_Stream.PushIndicesDataFast(1);
		else
			m_Stream.PushIndicesDataFast(i + 1);
		m_Stream.PushIndicesDataFast(i);
	}
	//bottom face
	for (int i = 1; i <= m_sectorCount; ++i)
	{
		m_Stream.PushIndicesDataFast(m_sectorCount + 1);
		m_Stream.PushIndicesDataFast(i);
		if (i == m_sectorCount)
			m_Stream.PushIndicesDataFast(1);
		else
			m_Stream.PushIndicesDataFast(i + 1);
	}
}

void ConeIndicesMetadata::ReleaseMetadate()
{
	m_Stream.Clear();
}

//
// ConeVertexMetadata
//
IMPLEMENT_RTTI(ConeVertexMetadata, VertexMetadata);
BEGIN_ADD_PROPERTY(ConeVertexMetadata, VertexMetadata);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(ConeVertexMetadata)
IMPLEMENT_INITIAL_END

ConeVertexMetadata::ConeVertexMetadata(RHIDefine::MemoryUseage mu, int sectorCount, float range, float angle,
	bool hasUV, bool hasNormal)
	: VertexMetadata(mu)
	, m_hasUV(hasUV)
	, m_hasNormal(hasNormal)
	, m_range(range)
	, m_angle(angle)
	, m_sectorCount(sectorCount)
{
}

void* ConeVertexMetadata::_ReturnMetadata()
{
	return &m_Stream;
}

void ConeVertexMetadata::GetIdentifier(std::string& hash) const
{
	hash.append(this->GetTypeName());
	hash.append(m_hasNormal ? "normal" : "none");
	hash.append(std::to_string(m_range));
	hash.append(std::to_string(m_angle));
	if (GetMemoryUseage() == RHIDefine::MU_DYNAMIC)
	{
		hash.append("dynamic");
	}
	else
	{
		hash.append("static");
	}
}

void ConeVertexMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
	int totalVertices = m_sectorCount + 2;
	m_Stream.ReserveBuffer(totalVertices);
	m_Stream.SetVertexType<Math::Vec4>(RHIDefine::PS_ATTRIBUTE_POSITION, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
	if (m_hasNormal)
	{
		m_Stream.SetVertexType<Math::Vec3>(RHIDefine::PS_ATTRIBUTE_NORMAL, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
	}
	if (m_hasUV)
	{
		m_Stream.SetVertexType<Math::Vec2>(RHIDefine::PS_ATTRIBUTE_COORDNATE0, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
	}

	float x, y, z;        // vertex position
	float nx, ny, nz;    // normal
	float s, t;         // texCoord

	float sectorStep = 2 * Math::PI / m_sectorCount;
	float sectorAngle = 0;
	float radius = tanf(m_angle * Math::PI / 180.0f) * m_range;
	float lengthInv = 1.0f / radius;

	//top point
	m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(0, 0, 0, 1).GetPtr());
	if (m_hasNormal)
	{
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(-1, 0, 0).GetPtr());
	}
	if (m_hasUV)
	{
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(0, 0).GetPtr());
	}

	for (int i = 0; i < m_sectorCount; ++i)
	{
		sectorAngle = i * sectorStep;
		x = radius * cosf(sectorAngle);
		y = radius * sinf(sectorAngle);
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(x, y, m_range, 1).GetPtr());

		if (m_hasNormal)
		{
			// normalized vertex normal
			nx = x * lengthInv;
			ny = y * lengthInv;
			m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(nx, ny, m_range).GetPtr());
		}

		if (m_hasUV)
		{
			// vertex tex coord between [0, 1]
			s = (float)i / m_sectorCount;
			t = (float)i / m_sectorCount;
			m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(s, t).GetPtr());
		}
	}

	m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(0, 0, m_range, 1).GetPtr());
	if (m_hasNormal)
	{
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(1, 0, 0).GetPtr());
	}
	if (m_hasUV)
	{
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(1, 1).GetPtr());
	}
}

void ConeVertexMetadata::ReleaseMetadate()
{
	m_Stream.Clear();
}

//
// QuadVertexMetadata
//
IMPLEMENT_RTTI(QuadVertexMetadata, VertexMetadata);
BEGIN_ADD_PROPERTY(QuadVertexMetadata, VertexMetadata);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(QuadVertexMetadata)
IMPLEMENT_INITIAL_END

QuadVertexMetadata::QuadVertexMetadata(RHIDefine::MemoryUseage mu, bool flip, bool flat)
	:VertexMetadata(mu)
	, m_isFlip(false)
	, m_isFlat(flat)
{
}

QuadVertexMetadata::QuadVertexMetadata(RHIDefine::MemoryUseage mu, bool flat)
	:VertexMetadata(mu)
	, m_isFlip(false)
	, m_isFlat(flat)
{
}

void* QuadVertexMetadata::_ReturnMetadata()
{
	return &m_Stream;
}

void QuadVertexMetadata::GetIdentifier(std::string& hash) const
{
	hash.append(this->GetTypeName());
	hash.append(m_isFlip ? "flip" : "original");
	hash.append(m_isFlat ? "flat" : "original");
	if (GetMemoryUseage() == RHIDefine::MU_DYNAMIC)
	{
		hash.append("dynamic");
	}
	else
	{
		hash.append("static");
	}
}

void QuadVertexMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
	m_Stream.SetVertexType<Math::Vec4>(RHIDefine::PS_ATTRIBUTE_POSITION, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
	m_Stream.SetVertexType<Math::Vec2>(RHIDefine::PS_ATTRIBUTE_COORDNATE0, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
	m_Stream.SetVertexType<Math::Vec3>(RHIDefine::PS_ATTRIBUTE_NORMAL, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
	m_Stream.ReserveBuffer(4);
	if (m_isFlat)
	{
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(-1, 0, 1, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(0, 1, 0).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(-1, 0, -1, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(0, 1, 0).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(1, 0, 1, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(0, 1, 0).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(1, 0, -1, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(0, 1, 0).GetPtr());
	}
	else
	{
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(-1, 1, 0, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(0, 0, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(-1, -1, 0, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(0, 0, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(1, 1, 0, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(0, 0, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(1, -1, 0, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(0, 0, 1).GetPtr());
	}
	if (m_isFlip)
	{
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(0, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(0, 0).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(1, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(1, 0).GetPtr());
	}
	else
	{
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(0, 0).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(0, 1).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(1, 0).GetPtr());
		m_Stream.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(1, 1).GetPtr());
	}
}

void QuadVertexMetadata::ReleaseMetadate()
{
	m_Stream.Clear();
}

//
// SquareIndicesMetadata
//
IMPLEMENT_RTTI(SquareIndicesMetadata, IndicesMetadata);
BEGIN_ADD_PROPERTY(SquareIndicesMetadata, IndicesMetadata);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(SquareIndicesMetadata)
IMPLEMENT_INITIAL_END

SquareIndicesMetadata::SquareIndicesMetadata(uint count)
	:IndicesMetadata(RHIDefine::MU_STATIC),
	m_Count(count)
{
}

SquareIndicesMetadata::SquareIndicesMetadata(uint count, RHIDefine::MemoryUseage mu)
	: IndicesMetadata(mu),
	m_Count(count)
{

}

SquareIndicesMetadata::~SquareIndicesMetadata(void)
{
}

void* SquareIndicesMetadata::_ReturnMetadata()
{
	return &m_Stream;
}

void SquareIndicesMetadata::GetIdentifier(std::string& hash) const
{
	hash.append(this->GetTypeName());
	if (GetMemoryUseage() == RHIDefine::MU_DYNAMIC)
	{
		hash.append("dynamic");
	}
	else
	{
		hash.append("static");
	}
}

void SquareIndicesMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)//处理原始资源
{
	m_Stream.SetIndicesType(RHIDefine::IT_UINT16);
	m_Stream.ReserveBuffer(m_Count * IC_INDICE_PREUI);
	for (int i = 0; i < m_Count; ++i)
	{
		m_Stream.PushIndicesDataFast(i * IC_VERTEX_PREUI + 0);
		m_Stream.PushIndicesDataFast(i * IC_VERTEX_PREUI + 1);
		m_Stream.PushIndicesDataFast(i * IC_VERTEX_PREUI + 2);

		m_Stream.PushIndicesDataFast(i * IC_VERTEX_PREUI + 2);
		m_Stream.PushIndicesDataFast(i * IC_VERTEX_PREUI + 1);
		m_Stream.PushIndicesDataFast(i * IC_VERTEX_PREUI + 3);

	}
}

void SquareIndicesMetadata::ReleaseMetadate()
{
	m_Stream.Clear();
}

NS_JYE_END