#pragma once

#include "Engine/private/Define.h"
#include "Engine/graphic/IAsyncResource.h"
#include "RHI/RHIDefine.h"
#include "Engine/resource/IAsyncEntity.h"
#include "Engine/graphic/IndicesBufferResource.h"
#include "Engine/resource/ISharedSource.h"

NS_JYE_BEGIN

class IndicesBufferSource : public ISharedSource
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	IndicesBufferSource();
	virtual ~IndicesBufferSource();

	FORCEINLINE RHIDefine::MemoryUseage GetMemoryUseage() { return m_eMemoryUseage; }
	FORCEINLINE void SetMemoryUseage(RHIDefine::MemoryUseage mu) { m_eMemoryUseage = mu; }
	FORCEINLINE void SetKeepSource(bool isKeepSource) { m_isKeepSource = isKeepSource; }
	FORCEINLINE bool isKeepSource() const { return m_isKeepSource; }

private:
	virtual IAsyncResource* _DoCreateResource() override;//将资源数据制作为实体资源
	virtual bool _DoSetupResource(IAsyncResource* res) override;//重新创建资源
	virtual bool _DoLoadResource() override;//加载资源

private:
	RHIDefine::MemoryUseage m_eMemoryUseage;
	bool m_isKeepSource;
};
DECLARE_Ptr(IndicesBufferSource);
TYPE_MARCO(IndicesBufferSource);

class ENGINE_API IndicesBufferEntity : public IAsyncEntity<IndicesBufferResource, IndicesBufferSource>
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	IndicesBufferEntity();
	virtual ~IndicesBufferEntity();
	DECLARE_OBJECT_CTOR_FUNC(IndicesBufferEntity);

	virtual void BeforeSave(Stream* pStream) override;
	virtual void PostSave(Stream* pStream) override;
	virtual void PostLoad(Stream* pStream) override;

	IndicesStream* IndicesBufferEntity::GetIndexStream()
	{
		if (_GetResourcePtr())
			return _GetResourcePtr()->GetIndexStream();
		return NULL;
	}
	FORCEINLINE uint GetIndicesCount()
	{
		return _GetResourcePtr()->GetIndicesCount();
	}
	FORCEINLINE bool FlushIndiesBuffer(const IndicesStream& buffer)
	{
		return _GetResourcePtr()->FlushIndicesBuffer(buffer, isKeepSource());
	}
	FORCEINLINE bool ChangeIndiesBuffer(const IndicesStream& buffer)
	{
		return _GetResourcePtr()->ChangeIndicesBuffer(buffer);
	}
	FORCEINLINE RHIDefine::IndicesType GetIndicesType() const
	{
		return _GetResourcePtr()->GetIndicesType();
	}
	FORCEINLINE void SetKeepSource(bool isKeepSource)
	{
		_GetSourcePtr()->SetKeepSource(isKeepSource);
		if (isKeepSource == false && _GetResourcePtr())
		{
			_GetResourcePtr()->UnloadSource();
		}
	}
	FORCEINLINE bool isKeepSource() const
	{
		return _GetSourcePtr()->isKeepSource();
	}
	FORCEINLINE handle GetGpuHandle() const
	{
		return _GetResourcePtr()->GetGpuHandle();
	}
	FORCEINLINE byte GetIndexTypeSize() const
	{
		return _GetResourcePtr()->GetIndexTypeSize();
	}
private:
	virtual void _OnCreateResource() override;

};
DECLARE_Ptr(IndicesBufferEntity);
TYPE_MARCO(IndicesBufferEntity);

NS_JYE_END