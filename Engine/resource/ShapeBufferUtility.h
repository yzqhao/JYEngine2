#include "VertexMetadata.h"
#include "IndicesMetadata.h"
#include "Math/3DMath.h"

NS_JYE_BEGIN

class ENGINE_API QuadIndicesMetadata : public IndicesMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	IndicesStream		m_Stream;
public:
	QuadIndicesMetadata() {}
	QuadIndicesMetadata(RHIDefine::MemoryUseage mu);
private:
	virtual void* _ReturnMetadata();
public:
	virtual void GetIdentifier(std::string& hash) const;
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash);//处理原始资源
	virtual void ReleaseMetadate();
};

class ENGINE_API QuadVertexMetadata : public VertexMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	VertexStream		m_Stream;
	bool				m_isFlip;
	bool				m_isFlat;
public:
	QuadVertexMetadata() {}
	//Depracated
	//parameter flip disabled in new material system
	//and y-flip issue handled from script by calling UniformNDC
	//no-matter what flip delivered, m_isFlip would be set to false
	QuadVertexMetadata(RHIDefine::MemoryUseage mu, bool flip, bool flat);

	QuadVertexMetadata(RHIDefine::MemoryUseage mu, bool flat);
private:
	virtual void* _ReturnMetadata();
public:
	virtual void GetIdentifier(std::string& hash) const;
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash);//处理原始资源
	virtual void ReleaseMetadate();
};

/**
* Indices meta data for a unit box.
* center at (0,0) with length 1.
*/
class ENGINE_API BoxIndicesMetadata : public IndicesMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	IndicesStream		m_Stream;
public:
	BoxIndicesMetadata() {}
	BoxIndicesMetadata(RHIDefine::MemoryUseage mu);
private:
	virtual void* _ReturnMetadata();
public:
	virtual void GetIdentifier(std::string& hash) const;
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash);//处理原始资源
	virtual void ReleaseMetadate();
};

/**
* Vertices meta data for a unit box.
* center at (0,0) with length 1.
*/
class ENGINE_API BoxVertexMetadata : public VertexMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	VertexStream		m_Stream;
	bool				m_hasNormal;
	bool				m_hasUV;
public:
	BoxVertexMetadata() {}
	/**
	* CTOR.
	* @param hasUV whether generate uvs for the mesh.
	* @param hasNormal whether generate normal for the mesh.
	*/
	BoxVertexMetadata(RHIDefine::MemoryUseage mu, bool hasUV = true, bool hasNormal = true);
private:
	virtual void* _ReturnMetadata();
public:
	virtual void GetIdentifier(std::string& hash) const;
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash);//处理原始资源
	virtual void ReleaseMetadate();
};

/**
* Indices meta data for a sphere. Must used with same sectorCount SphereVertexMetadata pair.
* center at (0,0).
*/
class ENGINE_API SphereIndicesMetadata : public IndicesMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	IndicesStream		m_Stream;
	int					m_sectorCount;
	int					m_stackCount;
public:
	SphereIndicesMetadata() {}
	/**
	* CTOR.
	* @param sectorCount Longtitude divided count. Higher count increase model accuracy also vertices size.
	* @param stackCount Latitude divided count. Higher count increase model accuracy also vertices size.
	*/
	SphereIndicesMetadata(RHIDefine::MemoryUseage mu, int sectorCount = 36, int stackCount = 18);
private:
	virtual void* _ReturnMetadata();
public:
	virtual void GetIdentifier(std::string& hash) const;
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash);//处理原始资源
	virtual void ReleaseMetadate();
};

/**
* Vertices meta data for a sphere.
* center at (0,0).
*/
class ENGINE_API SphereVertexMetadata : public VertexMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	VertexStream		m_Stream;
	bool				m_hasNormal;
	bool				m_hasUV;
	float				m_radius;
	int					m_sectorCount;
	int					m_stackCount;
public:
	SphereVertexMetadata() {}
	/**
	* CTOR.
	* @param radius The radius of the sphere
	* @param sectorCount Longtitude divided count. Higher count increase model accuracy also vertices size.
	* @param stackCount Latitude divided count. Higher count increase model accuracy also vertices size.
	* @param hasUV whether generate uvs for the mesh.
	* @param hasNormal whether generate normal for the mesh.
	*/
	SphereVertexMetadata(RHIDefine::MemoryUseage mu, float radius = 1.0f, int sectorCount = 36,
		int stackCount = 18, bool hasUV = true, bool hasNormal = true);
private:
	virtual void* _ReturnMetadata();
public:
	virtual void GetIdentifier(std::string& hash) const;
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash);//处理原始资源
	virtual void ReleaseMetadate();
};

/**
* Indices meta data for a cone. Must used with same sectorCount ConeVertexMetadata pair.
* Peak point located at (0,0), points towards outside screen(+z in opengl).
*/
class ENGINE_API ConeIndicesMetadata : public IndicesMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	IndicesStream		m_Stream;
	int					m_sectorCount;
public:
	ConeIndicesMetadata() {}
	/**
	* CTOR.
	* @param sectorCount triangulate count. Higher count increase model accuracy also vertices size.
	*/
	ConeIndicesMetadata(RHIDefine::MemoryUseage mu, int sectorCount = 180);
private:
	virtual void* _ReturnMetadata();
public:
	virtual void GetIdentifier(std::string& hash) const;
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash);//处理原始资源
	virtual void ReleaseMetadate();
};

/**
* Vertices meta data for a cone.
* Peak point located at (0,0), points towards outside screen(+z in opengl).
*/
class ENGINE_API ConeVertexMetadata : public VertexMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	VertexStream		m_Stream;
	int					m_sectorCount;
	float				m_range;
	float				m_angle;
	bool				m_hasNormal;
	bool				m_hasUV;
public:
	ConeVertexMetadata() {}
	/**
	* CTOR.
	* @param sectorCount triangulate count. Higher count increase model accuracy also vertices size.
	* @param range The distance between peak point and bottom face.
	* @param angle The half angle of the cone.
	* @param hasUV whether generate uvs for the mesh.
	* @param hasNormal whether generate normal for the mesh.
	*/
	ConeVertexMetadata(RHIDefine::MemoryUseage mu, int sectorCount = 180, float range = 1.0f,
		float angle = 45.0f, bool hasUV = true, bool hasNormal = true);
private:
	virtual void* _ReturnMetadata();
public:
	virtual void GetIdentifier(std::string& hash) const;
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash);//处理原始资源
	virtual void ReleaseMetadate();
};

class ENGINE_API SquareIndicesMetadata : public IndicesMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	enum Constant
	{
		IC_INDICE_PREUI = 6,
		IC_VERTEX_PREUI = 4,
	};
private:
	IndicesStream		m_Stream;
	uint				m_Count;
public:
	SquareIndicesMetadata() {}
	SquareIndicesMetadata(uint count);
	SquareIndicesMetadata(uint count, RHIDefine::MemoryUseage mu);
	~SquareIndicesMetadata(void);
private:
	virtual void* _ReturnMetadata();
public:
	virtual void GetIdentifier(std::string& hash) const;
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash);//处理原始资源
	virtual void ReleaseMetadate();
};

NS_JYE_END