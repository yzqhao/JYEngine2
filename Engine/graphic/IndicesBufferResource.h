#pragma once

#include "Engine/private/Define.h"
#include "RHI/RHIDefine.h"
#include "Engine/graphic/IAsyncResource.h"

NS_JYE_BEGIN

class IndicesStream;
class ENGINE_API IndicesBufferResource : public ISharedResource
{
private:
	RHIDefine::IndicesType m_eIndicesType;
	RHIDefine::MemoryUseage m_eMemoryUseage;
	IndicesStream* m_IndexStream;
	bool m_isKeepSource;
	uint m_GPUResourceHandle;
	uint m_uIndicesCount;//��������Ŀ
	byte m_TypeSize;
public:
	IndicesBufferResource();
	virtual ~IndicesBufferResource();
	virtual void ReleaseResource();//�ͷ���Դprivate:
private:
	bool _DeprecatedFilter(bool isd);
	bool _FlushIndicesBuffer(uint offset, uint indiesCount, uint typesize, const void* buffer);//��ԭ����bufferoffset��ʼ�����indiesCount�����ݣ�bufferΪ��Ҫ��������
public:
	void UnloadSource();
	IndicesStream* GetIndexStream();
	bool ChangeIndicesBuffer(const IndicesStream& buffer);
	bool FlushIndicesBuffer(const IndicesStream& buffer, bool isKeepSource);
public:
	
	FORCEINLINE uint GetIndicesCount()
	{
		return m_uIndicesCount;
	}
	FORCEINLINE void SetMemoryUseage(RHIDefine::MemoryUseage mu)
	{
		m_eMemoryUseage = mu;
	}
	FORCEINLINE RHIDefine::IndicesType GetIndicesType() const
	{
		return m_eIndicesType;
	}
	FORCEINLINE handle GetGpuHandle() const
	{
		return m_GPUResourceHandle;
	}
	FORCEINLINE byte GetIndexTypeSize() const
	{
		return (m_eIndicesType == RHIDefine::IndicesType::IT_UINT16) ? 2 : 4;
	}
};

NS_JYE_END