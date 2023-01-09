#pragma once

#include "Engine/private/Define.h"
#include "RHI/RHIDefine.h"
#include "Engine/graphic/IAsyncResource.h"
#include "Engine/render/VertexStreamLayout.h"

NS_JYE_BEGIN

class VertexStream;
class ENGINE_API VertexBufferResource : public ISharedResource
{
private:
	VertexStreamLayout m_VertexLayout;
	VertexStream* m_pVertexStream;
	RHIDefine::MemoryUseage m_eMemoryUseage;
	bool m_isKeepSource;
	handle m_handle;
	uint m_uVertexCount;//索引的数目
	uint m_uVertexSize;
public:
	VertexBufferResource();
	virtual ~VertexBufferResource();
	virtual void ReleaseResource();//释放资源
private:
	virtual bool _DeprecatedFilter(bool isd);

	bool _FlushVertexBuffer(const VertexStream& stream, bool isKeepSource, bool all = true);
protected:
	FORCEINLINE const VertexStreamLayout& _GetVertexStreamLayout() const { return m_VertexLayout; }
public:
	void UnloadSource();
	VertexStream* GetVertexStream();
	bool ChangeVertexBuffer(const VertexStream& stream);
	bool FlushVertexBuffer(const VertexStream& stream, bool isKeepSource);

	FORCEINLINE void SetMemoryUseage(RHIDefine::MemoryUseage mu)
	{
		m_eMemoryUseage = mu;
	}

	FORCEINLINE uint GetVertexSize() const
	{
		return m_uVertexSize;
	}

	FORCEINLINE uint GetVertexCount() const
	{
		return m_uVertexCount;
	}

	FORCEINLINE handle GetGpuHandle() const
	{
		return m_handle;
	}
};

NS_JYE_END